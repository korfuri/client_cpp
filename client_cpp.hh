#ifndef PROMETHEUS_CLIENT_HH__
# define PROMETHEUS_CLIENT_HH__

# include "arrayhash.hh"

# include <algorithm>
# include <array>
# include <atomic>
# include <mutex>
# include <ostream>
# include <string>
# include <type_traits>
# include <unordered_map>
# include <vector>

namespace prometheus {

  namespace impl {

    class AMetric;

    class OutputFormatter {
    public:
      OutputFormatter(std::ostream& os);

      void addMetric(std::string const& name,
		     std::string const& type) {
	os_ << "# TYPE " << type << std::endl;
      }

      template<typename LabelIterator>
      void addMetricRow(std::string const& name,
			LabelIterator const& labels_begin,
			LabelIterator const& labels_end,
			double value) {
	if (labels_begin == labels_end) {
	  os_ << name << " = " << value << std::endl;
	} else {
	  os_ << name;
	  char nextchar = '{';
	  for (auto it = labels_begin; it != labels_end; ++it) {
	    os_ << nextchar << it->first << '=' << it->second;
	    nextchar = ',';
	  }
	  os_ << "} = " << value << std::endl;
	}
      }

    private:
      std::ostream& os_;
    };

    class Registry {
    public:
      void register_metric(AMetric* metric);
      void output(std::ostream& os) const;
    private:
      std::vector<AMetric*> metrics_;
    };

    class AMetric {
    public:
      AMetric();
      AMetric(Registry* reg);
      virtual void output(std::ostream&) const = 0;
    };

    template<int N, class MetricType>
    class LabeledMetric : public AMetric {
      typedef std::array<std::string, N> stringarray;

    public:
      LabeledMetric(std::string const& name,
		    stringarray const& labelnames) :
	name_(name), labelnames_(labelnames) {
	static_assert(N >= 1, "A LabeledMetric should have at least 1 label.");
      }

      MetricType& labels(stringarray const& labelvalues) {
	std::unique_lock<std::mutex> l(mutex_);
	return values_[labelvalues];
      }

      virtual void output(std::ostream& os) const {
	os << "# TYPE gauge" << std::endl;
	std::unique_lock<std::mutex> l(mutex_);
	for (const auto& it_v : values_) {
	  os << name_;
	  char next_separator = '{';
	  auto labelname_it = labelnames_.begin();
	  for (const auto& it_l : it_v.first) {
	    os << next_separator << *labelname_it << "=" << it_l;
	    ++labelname_it;
	    next_separator = ',';
	  }
	  os << "} = ";
	  os << it_v.second.value();
	  os << std::endl;
	}
      }

    private:
      const std::string name_;
      stringarray const labelnames_;
      mutable std::mutex mutex_;
      std::unordered_map<stringarray, MetricType, ContainerHash<stringarray>, ContainerEq<stringarray>> values_;
    };

    template<class MetricType>
    class UnlabeledMetric : public AMetric, public MetricType {
    public:
      UnlabeledMetric(std::string const& name) : name_(name) {}

      virtual void output(std::ostream& os) const {
	os << "# TYPE gauge" << std::endl;
	os << name_ << " = " << this->value_ << std::endl;
      }

    private:
      std::string const name_;
    };

    class BaseScalarMetric {
    public:
      BaseScalarMetric() {}
      ~BaseScalarMetric() {}

      double value() const {
	return value_.load(std::memory_order_relaxed);
      }

    private:
      BaseScalarMetric(BaseScalarMetric const&) = delete;
      BaseScalarMetric(BaseScalarMetric&) = delete;
      BaseScalarMetric& operator=(BaseScalarMetric const&) = delete;
      BaseScalarMetric& operator=(BaseScalarMetric&&) = delete;

    protected:
      std::atomic<double> value_;
    };

    class Gauge : public BaseScalarMetric {
    public:
      void set(double value) {
	value_.store(value, std::memory_order_relaxed);
      }
    };


    class Counter : public BaseScalarMetric {
    public:
      void inc(double value = 1.0) {
	// if (value < 0) throw Something();
	double oldv, newv;
	do {
	  oldv = value_.load(std::memory_order_acquire);
	  newv = oldv + value;
	} while (value_.exchange(newv) != oldv);
      }
    };

  }  /* namespace impl */

  template<int N> class Gauge : public impl::LabeledMetric<N, impl::Gauge> {
    using impl::LabeledMetric<N, impl::Gauge>::LabeledMetric;
  };
  template<> class Gauge<0> : public impl::UnlabeledMetric<impl::Gauge> {
    using impl::UnlabeledMetric<impl::Gauge>::UnlabeledMetric;
  };

  template<int N> class Counter : public impl::LabeledMetric<N, impl::Counter> {
    using impl::LabeledMetric<N, impl::Counter>::LabeledMetric;
  };
  template<> class Counter<0> : public impl::UnlabeledMetric<impl::Counter> {
    using impl::UnlabeledMetric<impl::Counter>::UnlabeledMetric;
  };

  template<int N>
  class BaseHistogram : public impl::AMetric {
    typedef std::array<std::string, N+1> stringarray;
  protected:
    BaseHistogram(std::string const& name, std::vector<double> const& levels, stringarray const& labels) :
      counters_(name, labels),
      levels_(levels.size()),
      last_level_is_inf_(isposinf(levels[levels.size() - 1]))
    {
      std::transform(levels.begin(), levels.end(), levels_.begin(),
		     [](double d) { return std::make_pair(d, double_to_string(d)); });
    }

    static bool isposinf(double d) {
      return std::isinf(d) && d > 0;
    }

    static std::string double_to_string(double d) {
      // TODO(korfuri): Is this needed?
      if (isposinf(d)) { return "+Inf"; }
      return std::to_string(d);
    }

  public:
    virtual void output(std::ostream& os) const {
      counters_.output(os);
    }

  protected:
    Counter<N+1> counters_;
    std::vector<std::pair<double, std::string>> levels_;
    bool last_level_is_inf_;
  };

  template<unsigned long N, typename T = std::string>
  std::array<T, N+1> extend_array(std::array<T, N> const& ar, T const& v) {
    std::array<T, N+1> ret;
    std::copy(ar.begin(), ar.end(), ret.begin());
    ret[ret.size() - 1] = v;
    return ret;
  }

  template<int N>
  class Histogram : public BaseHistogram<N> {
    typedef std::array<std::string, N> stringarray;
  public:
    Histogram(std::string const& name, std::vector<double> const& levels, stringarray const& labels) :
      BaseHistogram<N>(name, levels, extend_array(labels, std::string("le"))) {}

    void add(double value, stringarray const& labels) {
      for (auto const& lvl : this->levels_) {
    	if (value > lvl.first) {
    	  this->counters_.labels(extend_array<N, std::string>(labels, lvl.second)).inc();
    	}
      }
      if (!this->last_level_is_inf_) {
    	this->counters_.labels(extend_array<N, std::string>(labels, "+Inf")).inc();
      }
    }
  };

  template<>
  class Histogram<0> : public BaseHistogram<0> {
  public:
    Histogram(std::string const& name, std::vector<double> const& levels) :
      BaseHistogram<0>(name, levels, {{"le"}}) {}

    void add(double value) {
      for (auto const& lvl : levels_) {
	if (value > lvl.first) {
	  counters_.labels({{lvl.second}}).inc();
	}
      }
      if (!last_level_is_inf_) {
	counters_.labels({{"+Inf"}}).inc();
      }
    }
  };

}  /* namespace prometheus */

#endif  /* PROMETHEUS_CLIENT_HH__ */
