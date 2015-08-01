#include "registry.hh"
#include "output_formatter.hh"

#include <mutex>

namespace prometheus {
  namespace impl {

    Registry global_registry;

    void Registry::register_metric(AbstractMetric* metric) {
      std::unique_lock<std::mutex> l(mutex_);
      metrics_.push_back(metric);
    }

    std::vector<MetricFamily*> Registry::output_proto() const {
      std::vector<MetricFamily*> v;
      std::unique_lock<std::mutex> l(mutex_);
      for (auto const m : metrics_) {
        auto* mf = new MetricFamily;
        m->output_proto(mf);
        v.push_back(mf);
      }
      return v;
    }

  } /* namespace impl */
} /* namespace prometheus */
