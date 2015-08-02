#include "client.hh"
#include "exceptions.hh"
#include "metrics.hh"
#include "prometheus/proto/metrics.pb.h"

#include <regex>

namespace prometheus {
  namespace impl {

    const std::regex label_name_re("^[a-zA-Z_:][a-zA-Z0-9_:]*$");
    const std::regex metric_name_re(
        "^((_[a-zA-Z0-9:])|[a-zA-Z:])[a-zA-Z0-9_:]*$");

    using ::io::prometheus::client::MetricFamily;

    AbstractMetric::AbstractMetric(const std::string& name,
                                   const std::string& help)
        : AbstractMetric(name, help, &global_registry) {
      if (!std::regex_match(name, metric_name_re)) {
        throw err::InvalidNameException();
      }
    }

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
