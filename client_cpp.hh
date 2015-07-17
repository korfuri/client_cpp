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

    template<int N, class MetricType>
    class LabeledMetric {
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

      void output(std::ostream& os) const {
	os << "# TYPE gauge" << std::endl;
	std::unique_lock<std::mutex> l(mutex_);
	for (const auto& it_v : values_) {
	  os << name_;
	  char next_separator = '{';
	  for (const auto& it_l : it_v.first) {
	    os << next_separator << "labelname_goes_here" << "=" << it_l;
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
    class UnlabeledMetric : public MetricType {
    public:
      UnlabeledMetric(std::string const& name) : name_(name) {}

      void output(std::ostream& os) const {
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
      BaseScalarMetric(BaseScalarMetric const&);
      BaseScalarMetric(BaseScalarMetric&);
      BaseScalarMetric& operator=(BaseScalarMetric const&);
      BaseScalarMetric& operator=(BaseScalarMetric&&);

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

  class Histogram {
  public:
    Histogram(std::string const& name, std::vector<double> const& levels) :
      counters_(name, {"le"}),
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

    void output(std::ostream& os) const {
      counters_.output(os);
    }

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

  private:
    Counter<1> counters_;
    std::vector<std::pair<double, std::string>> levels_;
    bool last_level_is_inf_;
  };

}  /* namespace prometheus */

#endif  /* PROMETHEUS_CLIENT_HH__ */
