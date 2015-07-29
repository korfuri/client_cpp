#include "client.hh"

#include <iostream>
#include <limits>

namespace prometheus {
  namespace impl {

    AbstractMetric::AbstractMetric(const std::string &name,
                                   const std::string &help)
        : AbstractMetric(name, help, &global_registry) {}

    AbstractMetric::AbstractMetric(const std::string &name,
                                   const std::string &help, Registry *reg)
        : name_(name), help_(help) {
      reg->register_metric(this);
    }

  } /* namespace impl */

  InProgress::InProgress(impl::IncDecGaugeValue& g, double value) :
    g_(g), value_(value) {
    g.inc(value_);
  }

  InProgress::~InProgress() {
    g_.dec(value_);
  }


} /* namespace prometheus */
