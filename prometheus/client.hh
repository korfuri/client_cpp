#ifndef PROMETHEUS_CLIENT_CPP_HH__
#define PROMETHEUS_CLIENT_CPP_HH__

#include "metrics.hh"
#include "registry.hh"
#include "output_formatter.hh"
#include "util/extend_array.hh"
#include "values.hh"

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

template <int N>
class BaseHistogram : public impl::AbstractMetric {
  typedef std::array<std::string, N + 1> stringarray;

 protected:
  BaseHistogram(std::string const& name, std::string const& help,
                std::vector<double> const& levels, stringarray const& labels)
      : AbstractMetric(name, help),
        counters_(name, help, labels),
        levels_(levels.size()),
        last_level_is_inf_(isposinf(levels[levels.size() - 1])) {
    std::transform(levels.begin(), levels.end(), levels_.begin(), [](double d) {
      return std::make_pair(d, double_to_string(d));
    });
  }

  static bool isposinf(double d) { return std::isinf(d) && d > 0; }

  static std::string double_to_string(double d) {
    // TODO(korfuri): Is this needed?
    if (isposinf(d)) {
      return "+Inf";
    }
    return std::to_string(d);
  }

 public:
  virtual void output(impl::OutputFormatter& f) const {
    f.addMetric(name_, help_, "histogram");
    counters_.value_output(f);
  }

 protected:
  Counter<N + 1> counters_;
  std::vector<std::pair<double, std::string>> levels_;
  bool last_level_is_inf_;
};

template <int N>
class Histogram : public BaseHistogram<N> {
  typedef std::array<std::string, N> stringarray;

 public:
  Histogram(std::string const& name, std::string const& help,
            std::vector<double> const& levels, stringarray const& labels)
      : BaseHistogram<N>(name, help, levels,
                         util::extend_array(labels, std::string("le"))) {}

  void add(double value, stringarray const& labels) {
    for (auto const& lvl : this->levels_) {
      if (value > lvl.first) {
        this->counters_.labels(util::extend_array<N, std::string>(
                                   labels, lvl.second)).inc();
      }
    }
    if (!this->last_level_is_inf_) {
      this->counters_.labels(util::extend_array<N, std::string>(labels, "+Inf"))
          .inc();
    }
  }
};

template <>
class Histogram<0> : public BaseHistogram<0> {
 public:
  Histogram(std::string const& name, std::string const& help,
            std::vector<double> const& levels)
      : BaseHistogram<0>(name, help, levels, {{"le"}}) {}

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

} /* namespace prometheus */

#endif /* PROMETHEUS_CLIENT_HH__ */
