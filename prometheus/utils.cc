#include "utils.hh"

#include <chrono>

namespace prometheus {

  InProgress::InProgress(impl::IncDecGaugeValue& g, double value)
      : g_(g), value_(value) {
    g.inc(value_);
  }

  InProgress::~InProgress() { g_.dec(value_); }

} /* namespace prometheus */
