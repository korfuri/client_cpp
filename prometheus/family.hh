#ifndef PROMETHEUS_METRIC_FAMILY_HH__
# define PROMETHEUS_METRIC_FAMILY_HH__

#include <memory>

#include "proto/stubs.hh"

namespace prometheus {

  using ::prometheus::client::MetricFamily;
  using MetricFamilyPtr = std::shared_ptr<MetricFamily>;

}; // end of namespace prometheus
#endif // PROMETHEUS_METRIC_FAMILY_HH__
