#ifndef PROMETHEUS_REGISTRY_HH__
#define PROMETHEUS_REGISTRY_HH__

#include "collector.hh"
#include "proto/stubs.hh"
#include "mutex.hh"

#include <ostream>
#include <list>
#include <vector>

namespace prometheus {
  namespace impl {

    class CollectorRegistry {
      // A CollectorRegistry is the main interface through which
      // collection happens. A CollectorRegistry contains a list of
      // Collectors, each of whom represents a collection of metrics.

     public:
      CollectorRegistry();
      ~CollectorRegistry();

      using collection_type = Collector::collection_type;

      // Returns a list of MetricFamily protobufs ready to be
      // exported. The called gain ownership of all allocated
      // MetricFamily objects and must delete them to avoid leaks.
      collection_type collect() const;

      // Register or unregister a collector. Registered collectors are
      // included in collections. Registering a collector twice, or
      // unregistering a collector that isn't registered, will throw a
      // CollectorManagementexception.
      void register_collector(ICollector* collector);
      void unregister_collector(ICollector* collector);

     private:
      CollectorRegistry(CollectorRegistry const&) = delete;
      CollectorRegistry operator=(CollectorRegistry const&) = delete;

      mutable impl::shared_timed_mutex mutex_;
      std::vector<ICollector*> collectors_;
    };

    // The global registry is available for clients to collect all
    // metrics.
    extern CollectorRegistry global_registry;

  } /* namespace impl */

} /* namespace prometheus */

#endif
