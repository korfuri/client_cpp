#ifndef PROMETHEUS_METRIC_FAMILY_HH__
# define PROMETHEUS_METRIC_FAMILY_HH__

#include <memory>
#include <list>

#include "proto/stubs.hh"

namespace prometheus {

  using ::prometheus::client::MetricFamily;
  using MetricFamilyPtr = std::shared_ptr<MetricFamily>;
  using collection_type = std::list<MetricFamilyPtr>;

}; // end of namespace prometheus
#endif // PROMETHEUS_METRIC_FAMILY_HH__
