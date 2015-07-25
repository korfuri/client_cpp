#include "registry.hh"
#include "output_formatter.hh"

namespace prometheus {
namespace impl {

Registry global_registry;

void Registry::register_metric(AbstractMetric* metric) {
  metrics_.push_back(metric);
}

void Registry::output(std::ostream& os) const {
  OutputFormatter f(os);
  for (auto const m : metrics_) {
    m->output(f);
  }
}

} /* namespace impl */
} /* namespace prometheus */
