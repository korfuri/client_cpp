#ifndef PROMETHEUS_CLIENT_CPP_HH__
#define PROMETHEUS_CLIENT_CPP_HH__

#include "exceptions.hh"
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
  class SetGauge : public impl::LabeledMetric<N, impl::SetGaugeValue> {
    using impl::LabeledMetric<N, impl::SetGaugeValue>::LabeledMetric;
  };
  template <>
  class SetGauge<0> : public impl::UnlabeledMetric<impl::SetGaugeValue> {
    using impl::UnlabeledMetric<impl::SetGaugeValue>::UnlabeledMetric;
  };

  template <int N>
  class IncDecGauge : public impl::LabeledMetric<N, impl::IncDecGaugeValue> {
    using impl::LabeledMetric<N, impl::IncDecGaugeValue>::LabeledMetric;
  };
  template <>
  class IncDecGauge<0> : public impl::UnlabeledMetric<impl::IncDecGaugeValue> {
    using impl::UnlabeledMetric<impl::IncDecGaugeValue>::UnlabeledMetric;
  };

  class InProgress {
  public:
    InProgress(impl::IncDecGaugeValue& g, double value = 1.0);
    ~InProgress();
  private:
    InProgress(InProgress const&) = delete;
    InProgress(InProgress&&) = delete;
    InProgress& operator=(InProgress const&) = delete;
    InProgress& operator=(InProgress&&) = delete;
    impl::IncDecGaugeValue& g_;
    double value_;
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

  // Use "+Inf" and "-Inf" to look like Go's extrema.
  extern const std::string kInfStr;
  extern const std::string kNegInfStr;

  std::string double_to_string(double d);

  template <int N>
  class BaseHistogram
      : public impl::LabeledMetric<N + 1, impl::HistogramValue> {
  private:
    typedef std::array<std::string, N + 1> stringarray;

  public:
    std::string level_up(double v) {
      // Returns the first level that contains a value v. This is
      // useful for testing, if you want to check that a certain value
      // is present. You can do:
      //  Histogram<0> h("h", "h");
      //  h.add(4.2);
      //  double x = h.labels({h.level_up(4.2)}).value();
      //  assert(x == 1.0);
      double previous = std::numeric_limits<double>::lowest();
      for (auto it = levels_.begin(); it != levels_.end(); ++it) {
	if (it->first >= v && v > previous) {
	  return it->second;
        }
      }
      // Should be unreachable
      return kInfStr;
    }

   protected:
    BaseHistogram(std::string const &name, std::string const &help,
                  std::vector<double> const &levels, stringarray const &labels)
        : impl::LabeledMetric<N + 1, impl::HistogramValue>(name, help, labels) {
      double last_level = std::numeric_limits<double>::lowest();
      for (auto const &l : levels) {
        if (l <= last_level) {
          throw err::UnsortedLevelsException();
        }
        levels_.push_back(std::make_pair(l, double_to_string(l)));
        last_level = l;
      }

      // Appends a +Inf level if there wasn't one.
      if (!std::isinf(last_level) || last_level < 0) {
        levels_.push_back(std::make_pair(kInf, kInfStr));
      }
    }

    std::vector<std::pair<double, std::string>> levels_;
  };

  template <int N>
  class Histogram : public BaseHistogram<N> {
    typedef std::array<std::string, N> stringarray;

   public:
    Histogram(std::string const &name, std::string const &help,
              stringarray const &labels,
              std::vector<double> const &levels = default_histogram_levels)
        : BaseHistogram<N>(name, help, levels,
                           util::extend_array(labels, std::string("le"))) {}

    void record(double value, stringarray const &labels) {
      for (auto const &lvl : this->levels_) {
	auto& row = this->labels(util::extend_array<N, std::string>(labels, lvl.second));
        if (value <= lvl.first) {
          row.inc();
        }
      }
    }
  };

  template <>
  class Histogram<0> : public BaseHistogram<0> {
   public:
    Histogram(std::string const &name, std::string const &help,
              std::vector<double> const &levels = default_histogram_levels)
        : BaseHistogram<0>(name, help, levels, {{"le"}}) {}

    void record(double value) {
      for (auto const &lvl : levels_) {
	auto& row = this->labels({{lvl.second}});
        if (value <= lvl.first) {
          row.inc();
        }
      }
    }
  };

} /* namespace prometheus */

#endif /* PROMETHEUS_CLIENT_HH__ */
