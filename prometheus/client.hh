#ifndef PROMETHEUS_CLIENT_CPP_HH__
#define PROMETHEUS_CLIENT_CPP_HH__

#include "exceptions.hh"
#include "metrics.hh"
#include "registry.hh"
#include "util/extend_array.hh"
#include "values.hh"
#include "policy.hh"

#include <array>
#include <string>
#include <vector>

namespace prometheus {

  template <int N>
  class SetGauge : public impl::Metric<N, impl::SetGaugeValue> {
    using impl::Metric<N, impl::SetGaugeValue>::Metric;
  };

  template <int N>
  class IncDecGauge : public impl::Metric<N, impl::IncDecGaugeValue> {
    using impl::Metric<N, impl::IncDecGaugeValue>::Metric;
  };

  template <int N>
  class Counter : public impl::Metric<N, impl::CounterValue> {
    using impl::Metric<N, impl::CounterValue>::Metric;
  };

  template <int N>
  class Histogram : public impl::Metric<N, impl::HistogramValue> {
    using impl::Metric<N, impl::HistogramValue>::Metric;
  };

  template <size_t Dims>
  class Gauge : public impl::Metric<Dims, impl::GaugeValue> {
    using impl::Metric<Dims, impl::GaugeValue>::Metric;
  };
  
  template <typename ValueType, typename... Args>
  impl::Metric<impl::get_dims<Args...>::value, ValueType>
  makeMetric(std::string const& name, std::string const& help, Args &&... args) {
    using metric_type = impl::Metric<impl::get_dims<Args...>::value, ValueType>;
    return metric_type(name, help, args...);
  }

  template <typename... Args>
  impl::Metric<impl::get_dims<Args...>::value, impl::CounterValue>
  makeCounter(std::string const& name, std::string const& help, Args &&... args) {
    return makeMetric<impl::CounterValue>(name, help, std::forward<Args>(args)...);
  }

} /* namespace prometheus */

#endif /* PROMETHEUS_CLIENT_HH__ */
