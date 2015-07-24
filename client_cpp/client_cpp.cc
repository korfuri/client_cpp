#include "client_cpp.hh"

#include <iostream>

namespace prometheus {
  namespace impl {

    Registry global_registry;

    void Registry::register_metric(AMetric* metric) {
      metrics_.push_back(metric);
    }

    void Registry::output(std::ostream& os) const {
      //OutputFormatter f(os);
      for (auto const m : metrics_) {
	m->output(os);
      }
    }

    AMetric::AMetric() : AMetric(&global_registry) {}

    AMetric::AMetric(Registry* reg) {
      reg->register_metric(this);
    }

  }  /* namespace impl */
}  /* namespace prometheus */
