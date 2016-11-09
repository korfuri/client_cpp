#include "collector.hh"
#include "metrics.hh"
#include "registry.hh"
#include "prometheus/proto/metrics.pb.h"
#include "mutex.hh"

#include <list>

namespace prometheus {
  namespace impl {

    CollectorRegistry global_registry;
    Collector global_collector(global_registry);

    Collector::Collector(CollectorRegistry& registry) {
      registry.register_collector(this);
    }

    Collector::~Collector() {}

    void Collector::register_metric(AbstractMetric* metric) {
      std::unique_lock<impl::shared_timed_mutex> l(mutex_);
      metrics_.push_back(metric);
    }

    Collector::collection_type Collector::collect() const {
      collection_type v;
      impl::shared_lock<impl::shared_timed_mutex> l(mutex_);
      for (auto const m : metrics_) {
        auto* mf = new MetricFamily;
        m->collect(mf);
        v.push_back(MetricFamilyPtr(mf));
      }
      return v;
    }

  } /* namespace impl */
} /* namespace prometheus */
