#ifndef PROMETHEUS_CLIENT_HH__
# define PROMETHEUS_CLIENT_HH__

# include "arrayhash.hh"

# include <array>
# include <atomic>
# include <mutex>
# include <ostream>
# include <string>
# include <type_traits>
# include <unordered_map>

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

class Gauge {
public:
  Gauge() {}
  ~Gauge() {}

  void set(double value) {
    value_.store(value);
  }

  double value() const {
    return value_.load();
  }

protected:
  std::atomic<double> value_;
};


class Counter {
public:
  Counter() {}
  ~Counter() {}

  void inc(double value = 1.0) {
    // if (value < 0) throw Something();
    double oldv, newv;
    do {
      oldv = value_.load();
      newv = oldv + value;
    } while (value_.exchange(newv) != oldv);
  }

  double value() const {
    return value_.load();
  }

protected:
  std::atomic<double> value_;
};

#endif  /* PROMETHEUS_CLIENT_HH__ */
