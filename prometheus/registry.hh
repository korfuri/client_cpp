#ifndef PROMETHEUS_REGISTRY_HH__
#define PROMETHEUS_REGISTRY_HH__

#include "metrics.hh"

#include <ostream>
#include <vector>

namespace prometheus {
namespace impl {

class Registry {
 public:
  void register_metric(AbstractMetric* metric);
  void output(std::ostream& os) const;

 private:
  std::vector<AbstractMetric*> metrics_;
};

extern Registry global_registry;

} /* namespace impl */
} /* namespace prometheus */

#endif
