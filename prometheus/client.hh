#ifndef PROMETHEUS_CLIENT_CPP_HH__
#define PROMETHEUS_CLIENT_CPP_HH__

#include "exceptions.hh"
#include "metrics.hh"
#include "registry.hh"
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

  template <int N>
  class Counter : public impl::LabeledMetric<N, impl::CounterValue> {
    using impl::LabeledMetric<N, impl::CounterValue>::LabeledMetric;
  };
  template <>
  class Counter<0> : public impl::UnlabeledMetric<impl::CounterValue> {
    using impl::UnlabeledMetric<impl::CounterValue>::UnlabeledMetric;
  };

  template <int N>
  class Histogram : public impl::LabeledMetric<N, impl::HistogramValue> {
    using impl::LabeledMetric<N, impl::HistogramValue>::LabeledMetric;
  };
  template <>
  class Histogram<0> : public impl::UnlabeledMetric<impl::HistogramValue> {
    using impl::UnlabeledMetric<impl::HistogramValue>::UnlabeledMetric;
  };

  template <typename... Args>
  std::array<std::string, sizeof...(Args)>
  labels(Args... args) {
    return {args...};
  }

  template <size_t Dims>
  Counter<Dims>
  makeCounter(std::string const& name, std::string const& help,
              std::array<std::string, Dims> const& lables)
  {
    return Counter<Dims>(name, help, lables);
  }

  inline
  Counter<0>
  makeCounter(std::string const& name, std::string const& help) {
    return Counter<0>(name, help);
  }

  
} /* namespace prometheus */

#endif /* PROMETHEUS_CLIENT_HH__ */
