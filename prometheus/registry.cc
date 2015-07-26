#include "registry.hh"
#include "output_formatter.hh"

#include <mutex>

namespace prometheus {
namespace impl {

Registry global_registry;

void Registry::register_metric(AbstractMetric* metric) {
  std::unique_lock<std::mutex> l(mutex_);
  metrics_.push_back(metric);
}

void Registry::output(std::ostream& os) const {
  OutputFormatter f(os);
  std::unique_lock<std::mutex> l(mutex_);
  for (auto const m : metrics_) {
    m->output(f);
  }
}

} /* namespace impl */
} /* namespace prometheus */
