#ifndef PROMETHEUS_METRICS_HH__
#define PROMETHEUS_METRICS_HH__

#include "util/container_hash.hh"
#include "output_formatter.hh"
#include "util/zipped_iterator.hh"

#include <algorithm>
#include <array>
#include <mutex>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace prometheus {
  namespace impl {

    class Registry;

    class AbstractMetric {
     public:
      AbstractMetric(const std::string &name, const std::string &help);
      AbstractMetric(const std::string &name, const std::string &help,
                     Registry *reg);
      virtual void output(OutputFormatter &) const = 0;

     protected:
      std::string name_;
      std::string help_;
    };

    template <int N, class MetricType>
    class LabeledMetric : public AbstractMetric {
      typedef std::array<std::string, N> stringarray;

     public:
      LabeledMetric(std::string const &name, std::string const &help,
                    stringarray const &labelnames)
          : AbstractMetric(name, help), labelnames_(labelnames) {
        static_assert(N >= 1, "A LabeledMetric should have at least 1 label.");
      }

      MetricType &labels(stringarray const &labelvalues) {
        std::unique_lock<std::mutex> l(mutex_);
        return values_[labelvalues];
      }

      virtual void output(OutputFormatter &f) const {
        f.addMetric(name_, help_, MetricType::type_);
        std::unique_lock<std::mutex> l(mutex_);
        for (const auto &it_v : values_) {
          auto zbegin =
              util::zip_iterators(labelnames_.begin(), it_v.first.begin());
          auto zend = util::zip_iterators(labelnames_.end(), it_v.first.end());
          f.addMetricLabelRow(name_, zbegin, zend, it_v.second.value());
        }
      }

     private:
      stringarray const labelnames_;
      mutable std::mutex mutex_;
      std::unordered_map<stringarray, MetricType,
                         util::ContainerHash<stringarray>,
                         util::ContainerEq<stringarray>> values_;
    };

    template <class MetricType>
    class UnlabeledMetric : public AbstractMetric, public MetricType {
     public:
      UnlabeledMetric(std::string const &name, std::string const &help)
          : AbstractMetric(name, help) {}

      virtual void output(OutputFormatter &f) const {
        f.addMetric(name_, help_, MetricType::type_);
        f.addMetricValue(name_, this->value_);
      }
    };

  } /* namespace impl */
} /* namespace prometheus */

#endif
