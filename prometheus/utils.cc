#include "utils.hh"

#include <chrono>

namespace prometheus {

  std::string make_metric_name(std::string const& namespace_name,
			       std::string const& subsystem_name,
			       std::string const& metric_name) {
    return namespace_name + "_" + subsystem_name + "_" + metric_name;
  }

  InProgress::InProgress(impl::IncDecGaugeValue& g, double value)
      : g_(g), value_(value) {
    g.inc(value_);
  }

  InProgress::~InProgress() { g_.dec(value_); }

} /* namespace prometheus */
