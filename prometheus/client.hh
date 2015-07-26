#ifndef PROMETHEUS_CLIENT_CPP_HH__
#define PROMETHEUS_CLIENT_CPP_HH__

#include "metrics.hh"
#include "registry.hh"
#include "output_formatter.hh"
#include "util/extend_array.hh"
#include "values.hh"

#include <array>
#include <string>
#include <vector>

namespace prometheus {

template <int N>
class Gauge : public impl::LabeledMetric<N, impl::GaugeValue> {
  using impl::LabeledMetric<N, impl::GaugeValue>::LabeledMetric;
};
template <>
class Gauge<0> : public impl::UnlabeledMetric<impl::GaugeValue> {
  using impl::UnlabeledMetric<impl::GaugeValue>::UnlabeledMetric;
};

template <int N>
class Counter : public impl::LabeledMetric<N, impl::CounterValue> {
  using impl::LabeledMetric<N, impl::CounterValue>::LabeledMetric;
};
template <>
class Counter<0> : public impl::UnlabeledMetric<impl::CounterValue> {
  using impl::UnlabeledMetric<impl::CounterValue>::UnlabeledMetric;
};

extern const std::vector<double> default_histogram_levels;

// Always equal to std::numeric_limits<double>::infinity().
// Provided only for convenience.
extern const double kInf;

extern const std::string kInfStr;

template <int N>
class BaseHistogram : public impl::LabeledMetric<N + 1, impl::HistogramValue> {
  typedef std::array<std::string, N + 1> stringarray;

 protected:
  BaseHistogram(std::string const& name, std::string const& help,
                std::vector<double> const& levels, stringarray const& labels)
      : impl::LabeledMetric<N + 1, impl::HistogramValue>(name, help, labels) {
    double last_level = std::numeric_limits<double>::min();
    for (auto const& l : levels) {
      // TODO(korfuri): if (l <= last_level) throw Something();
      levels_.push_back(std::make_pair(l, double_to_string(l)));
      last_level = l;
    }

    // Appends a +Inf level if there wasn't one.
    if (!isposinf(last_level)) {
      levels_.push_back(std::make_pair(kInf, kInfStr));
    }
  }

  static bool isposinf(double d) { return std::isinf(d) && d > 0; }

  static std::string double_to_string(double d) {
    // TODO(korfuri): Is this needed?
    if (isposinf(d)) {
      return kInfStr;
    }
    // TODO(korfuri): Try to have a friendlier formatting, 1.0 is
    // formatted as "1.0000000" with (my system's) std::to_string.
    return std::to_string(d);
  }

  std::string level_up(double v) {
    // Returns the first level that contains a value v. This is
    // useful for testing, if you want to check that a certain value
    // is present. You can do:
    //  Histogram<0> h("h", "h");
    //  h.add(4.2);
    //  double x = h.labels({h.level_up(4.2)}).value();
    //  assert(x == 1.0);
    for (auto it = levels_.begin(); it != levels_.end(); ++it) {
      if (it->first < v) {
        ++it;
        if (it == levels_.end()) {
          return kInfStr;
        } else {
          return it->second;
        }
      }
      return kInfStr;
    }
  }

 protected:
  std::vector<std::pair<double, std::string>> levels_;
};

template <int N>
class Histogram : public BaseHistogram<N> {
  typedef std::array<std::string, N> stringarray;

 public:
  Histogram(std::string const& name, std::string const& help,
            stringarray const& labels,
            std::vector<double> const& levels = default_histogram_levels)
      : BaseHistogram<N>(name, help, levels,
                         util::extend_array(labels, std::string("le"))) {}

  void record(double value, stringarray const& labels) {
    for (auto const& lvl : this->levels_) {
      if (value <= lvl.first) {
        this->labels(util::extend_array<N, std::string>(labels, lvl.second))
            .inc();
      }
    }
  }
};

template <>
class Histogram<0> : public BaseHistogram<0> {
 public:
  Histogram(std::string const& name, std::string const& help,
            std::vector<double> const& levels = default_histogram_levels)
      : BaseHistogram<0>(name, help, levels, {{"le"}}) {}

  void record(double value) {
    for (auto const& lvl : levels_) {
      if (value <= lvl.first) {
        this->labels({{lvl.second}}).inc();
      }
    }
  }
};

} /* namespace prometheus */

#endif /* PROMETHEUS_CLIENT_HH__ */
