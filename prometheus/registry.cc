#include "registry.hh"
#include "client.hh"
#include "exceptions.hh"
#include "prometheus/proto/metrics.pb.h"

#include <algorithm>
#include <mutex>
#include <shared_mutex>
#include <vector>

namespace prometheus {
  namespace impl {

    Counter<0> collection_errors(
      "prometheus_client_collection_errors_total",
      ("Count of exceptions raised by collectors during the metric"
       " collection process."));

    CollectorRegistry::CollectorRegistry() {}
    CollectorRegistry::~CollectorRegistry() {}

    void CollectorRegistry::register_collector(ICollector* collector) {
      std::unique_lock<std::shared_timed_mutex> l(mutex_);
      if (std::find(collectors_.begin(), collectors_.end(), collector) !=
          collectors_.end()) {
        throw err::CollectorManagementException();
      }
      collectors_.push_back(collector);
    }

    void CollectorRegistry::unregister_collector(ICollector* collector) {
      std::unique_lock<std::shared_timed_mutex> l(mutex_);
      auto it = std::find(collectors_.begin(), collectors_.end(), collector);
      if (it == collectors_.end()) {
        throw err::CollectorManagementException();
      }
      collectors_.erase(it);
    }

    std::list<MetricFamily*> CollectorRegistry::collect() const {
      std::shared_lock<std::shared_timed_mutex> l(mutex_);
      std::list<MetricFamily*> metrics;
      for (auto const& c : collectors_) {
	try {
	  std::list<MetricFamily*> collected_metrics = c->collect();
	  metrics.splice(metrics.begin(), collected_metrics);
	} catch (CollectionException const&) {
	  collection_errors.inc();
	}
      }
      return metrics;
    }

  } /* namespace impl */

  void delete_metricfamily(::prometheus::client::MetricFamily* mf) {
    delete mf;
  }
} /* namespace prometheus */
