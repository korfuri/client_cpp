#include "registry.hh"
#include "exceptions.hh"
#include "prometheus/proto/metrics.pb.h"

#include <algorithm>
#include <mutex>
#include <vector>

namespace prometheus {
  namespace impl {

    CollectorRegistry::CollectorRegistry() {}
    CollectorRegistry::~CollectorRegistry() {}

    void CollectorRegistry::register_collector(ICollector* collector) {
      std::unique_lock<std::mutex> l(mutex_);
      if (std::find(collectors_.begin(), collectors_.end(), collector) !=
          collectors_.end()) {
        throw err::CollectorManagementException();
      }
      collectors_.push_back(collector);
    }

    void CollectorRegistry::unregister_collector(ICollector* collector) {
      std::unique_lock<std::mutex> l(mutex_);
      auto it = std::find(collectors_.begin(), collectors_.end(), collector);
      if (it == collectors_.end()) {
        throw err::CollectorManagementException();
      }
      collectors_.erase(it);
    }

    std::list<MetricFamily*> CollectorRegistry::collect() const {
      std::unique_lock<std::mutex> l(mutex_);
      std::list<MetricFamily*> metrics;
      for (auto const& c : collectors_) {
        std::list<MetricFamily*> collected_metrics = c->collect();
        metrics.splice(metrics.begin(), collected_metrics);
      }
      return metrics;
    }

  } /* namespace impl */
} /* namespace prometheus */
