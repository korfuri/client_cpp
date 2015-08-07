#ifndef PROMETHEUS_COLLECTOR_HH_
#define PROMETHEUS_COLLECTOR_HH_

#include "proto/stubs.hh"

#include <mutex>
#include <list>
#include <vector>

namespace prometheus {

  namespace impl {
    using ::io::prometheus::client::MetricFamily;

    class AbstractMetric;
    class CollectorRegistry;
  }

  class ICollector {
    // This is the interface for collectors. Most users don't need to
    // define their own collector, there is a global ProcessCollector
    // at prometheus::impl::global_process_collector, and metrics
    // default to registering with that collector. The main use case
    // to define your own collector is if you're re-exporting metrics
    // collected from another process.

   public:
    virtual ~ICollector() {}

    // Returns a list of MetricFamily protobufs ready to be
    // exported. The called gain ownership of all allocated
    // MetricFamily objects. This method must be thread-safe.
    virtual std::list<impl::MetricFamily*> collect() const = 0;
  };

  namespace impl {

    class ProcessCollector : public ICollector {
     public:
      // Constructing a ProcessCollector registers that collector with
      // a CollectorRegistry. Destroying the ProcessCollector does NOT
      // unregister it automatically, it is up to the caller to
      // unregister the ProcessCollector first.
      ProcessCollector(impl::CollectorRegistry& registry);
      virtual ~ProcessCollector();

      // See ICollector::collect.
      virtual std::list<MetricFamily*> collect() const;

      // Registers a metric with this ProcessCollector. The metric
      // can't be unregistered.
      void register_metric(AbstractMetric* metric);

     private:
      ProcessCollector(ProcessCollector const&) = delete;
      ProcessCollector& operator=(ProcessCollector const&) = delete;

      std::vector<AbstractMetric*> metrics_;
      mutable std::mutex mutex_;
    };

    extern ProcessCollector global_process_collector;
  } /* namespace impl */
} /* namespace prometheus */

#endif
