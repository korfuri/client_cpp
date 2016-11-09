#include "client.hh"
#include "exceptions.hh"
#include "metrics.hh"
#include "prometheus/proto/metrics.pb.h"

#include <regex>

namespace prometheus {
  namespace impl {

    using ::prometheus::client::MetricFamily;

    AbstractMetric::AbstractMetric(const std::string& name,
                                   const std::string& help,
                                   Collector* collector)
        : name_(name), help_(help) {
      const std::regex metric_name_re("^((_[a-zA-Z0-9:])|[a-zA-Z:])[a-zA-Z0-9_:]*$");
      if (!std::regex_match(name_, metric_name_re)) {
        throw err::InvalidNameException();
      }
      collector->register_metric(this);
    }

    void AbstractMetric::collect_internal(client::MetricFamily* mf) const {
      mf->set_name(name_);
      mf->set_help(help_);
    }

    client::Metric* AbstractMetric::add_metric(client::MetricFamily* mf) {
      return mf->add_metric();
    }
    client::LabelPair* AbstractMetric::add_label(client::Metric* m) {
      return m->add_label();
    }
    void AbstractMetric::set_label(client::LabelPair* l,
                                   std::string const& name,
                                   std::string const& value) {
      l->set_name(name);
      l->set_value(value);
    }

  } /* namespace impl */
} /* namespace prometheus */
