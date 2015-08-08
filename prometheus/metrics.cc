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
                                   const std::string& help,
                                   Collector* collector)
        : name_(name), help_(help) {
      if (!std::regex_match(name, metric_name_re)) {
        throw err::InvalidNameException();
      }
      collector->register_metric(this);
    }

    void AbstractMetric::collect_internal(MetricFamily* mf) const {
      mf->set_name(name_);
      mf->set_help(help_);
    }

    /* static */ Metric* AbstractMetric::add_metric(MetricFamily* mf) {
      return mf->add_metric();
    }
    /* static */ LabelPair* AbstractMetric::add_label(Metric* m) {
      return m->add_label();
    }
    /* static */ void AbstractMetric::set_label(LabelPair* l,
                                                std::string const& name,
                                                std::string const& value) {
      l->set_name(name);
      l->set_value(value);
    }

  } /* namespace impl */
} /* namespace prometheus */
