#include "client.hh"

#include <iostream>

namespace prometheus {
namespace impl {

AbstractMetric::AbstractMetric(const std::string& name, const std::string& help)
    : AbstractMetric(name, help, &global_registry) {}

AbstractMetric::AbstractMetric(const std::string& name, const std::string& help,
                               Registry* reg)
    : name_(name), help_(help) {
  reg->register_metric(this);
}

} /* namespace impl */
} /* namespace prometheus */
