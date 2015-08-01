#include "client.hh"
#include "metrics.hh"
#include "prometheus/proto/metrics.pb.h"

namespace prometheus {
  namespace impl {

    using ::io::prometheus::client::MetricFamily;

    AbstractMetric::AbstractMetric(const std::string& name,
                                   const std::string& help)
        : AbstractMetric(name, help, &global_registry) {}

    AbstractMetric::AbstractMetric(const std::string& name,
                                   const std::string& help, Registry* reg)
        : name_(name), help_(help) {
      reg->register_metric(this);
    }

    void AbstractMetric::output_proto_internal(MetricFamily* mf) const {
      mf->set_name(name_);
      mf->set_help(help_);
    }

  } /* namespace impl */
} /* namespace prometheus */
