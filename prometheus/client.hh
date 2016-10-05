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
  
  template <typename... Args>
  std::array<std::string, sizeof...(Args)>
  labels(Args const& ... args) {
    return {args...};
  }

  // TODO(@agnat): remove when labels become a policy
  template <size_t Dims, typename... ValueArgs>
  Counter<Dims>
  makeCounter(std::string const& name, std::string const& help,
              std::array<std::string, Dims> const& lables,
              ValueArgs const&... args)
  {
    return Counter<Dims>(name, help, lables, args...);
  }

  // TODO(@agnat): needs perfect forwarding when labels become a policy
  template <typename... ValueArgs>
  Counter<0>
  makeCounter(std::string const& name, std::string const& help, 
              ValueArgs const&... args)
  {
    return Counter<0>(name, help, args...);
  }

  
} /* namespace prometheus */

#endif /* PROMETHEUS_CLIENT_HH__ */
