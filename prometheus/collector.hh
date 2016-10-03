#ifndef PROMETHEUS_COLLECTOR_HH_
#define PROMETHEUS_COLLECTOR_HH_

#include "family.hh"
#include "mutex.hh"

#include <list>
#include <stdexcept>
#include <vector>

namespace prometheus {

  namespace impl {
    class AbstractMetric;
    class CollectorRegistry;
  }

  class ICollector {
    // This is the interface for collectors. Most users don't need to
    // define their own collector, there is a global Collector at
    // prometheus::impl::global_collector, and metrics default to
    // registering with that collector. The main use case to define
    // your own collector is if you're generating metrics from a data
    // source such as /proc, or another process' metrics.

   public:
    virtual ~ICollector() {}

    typedef std::list<MetricFamilyPtr> collection_type;

    // Returns a list of MetricFamily protobufs ready to be
    // exported. The called gain ownership of all allocated
    // MetricFamily objects. This method must be thread-safe. It may
    // throw a CollectionException to signify that collection could
    // not happen at this time. The CollectionException should provide
    // a what() string explaining the reason collection couldn't
    // happen. Other exceptions will interrupt the registry's
    // collection process and no metrics will be exposed for this
    // collection of the whole registry.
    virtual collection_type collect() const = 0;
  };

  class CollectionException : public std::runtime_error {};

  namespace impl {

    class Collector : public ICollector {
     public:
      // Constructing a Collector registers that collector with
      // a CollectorRegistry. Destroying the Collector does NOT
      // unregister it automatically, it is up to the caller to
      // unregister the Collector first.
      Collector(impl::CollectorRegistry& registry);
      virtual ~Collector();

      // See ICollector::collect.
      virtual collection_type collect() const;

      // Registers a metric with this Collector. The metric
      // can't be unregistered.
      void register_metric(AbstractMetric* metric);

     private:
      Collector(Collector const&) = delete;
      Collector& operator=(Collector const&) = delete;

      std::vector<AbstractMetric*> metrics_;
      mutable impl::shared_timed_mutex mutex_;
    };

    extern Collector global_collector;
  } /* namespace impl */
} /* namespace prometheus */

#endif
