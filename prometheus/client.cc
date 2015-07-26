#include "client.hh"

#include <iostream>
#include <limits>

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

const double kInf = std::numeric_limits<double>::infinity();
const std::string kInfStr = "+Inf";
const std::vector<double> default_histogram_levels = {
    .005, .01, .025, .05, .075, .1,   .25, .5,
    .75,  1.0, 2.5,  5.0, 7.5,  10.0, kInf};

} /* namespace prometheus */
