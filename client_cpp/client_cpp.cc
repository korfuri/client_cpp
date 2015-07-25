#include "client_cpp.hh"

#include <iostream>

namespace prometheus {
namespace impl {

AbstractMetric::AbstractMetric() : AbstractMetric(&global_registry) {}

AbstractMetric::AbstractMetric(Registry* reg) { reg->register_metric(this); }

} /* namespace impl */
} /* namespace prometheus */
