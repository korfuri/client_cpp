#include "collector.hh"
#include "metrics.hh"
#include "registry.hh"
#include "prometheus/proto/metrics.pb.h"

#include <mutex>
#include <list>

namespace prometheus {
  namespace impl {

    CollectorRegistry global_registry;
    ProcessCollector global_process_collector(global_registry);

    ProcessCollector::ProcessCollector(CollectorRegistry& registry) {
      registry.register_collector(this);
    }

    ProcessCollector::~ProcessCollector() {}

    void ProcessCollector::register_metric(AbstractMetric* metric) {
      std::unique_lock<std::mutex> l(mutex_);
      metrics_.push_back(metric);
    }

    std::list<MetricFamily*> ProcessCollector::collect() const {
      std::list<MetricFamily*> v;
      std::unique_lock<std::mutex> l(mutex_);
      for (auto const m : metrics_) {
        auto* mf = new MetricFamily;
        m->collect(mf);
        v.push_back(mf);
      }
      return v;
    }

  } /* namespace impl */
} /* namespace prometheus */
