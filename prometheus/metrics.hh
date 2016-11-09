#ifndef PROMETHEUS_METRICS_HH__
#define PROMETHEUS_METRICS_HH__

#include "collector.hh"
#include "exceptions.hh"
#include "proto/stubs.hh"
#include "util/container_hash.hh"
#include "util/zipped_iterator.hh"

#include <algorithm>
#include <array>
#include <mutex>
#include <regex>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace prometheus {
  namespace impl {

    class Registry;

    class AbstractMetric {
      // This is the base class for all metrics. Metrics are either
      // labelled or not, and they have an underlying value type
      // (counter, histogram, etc.). client.hh provides convenient
      // template overloads to use Counter, Gauge, Histogram as
      // classes with or without labels.
     public:
      AbstractMetric(const std::string& name, const std::string& help,
                     Collector* collector);

      // All metrics can be collected to a MetricFamily protobuf
      // object.
      virtual void collect(MetricFamily* mf) const = 0;

     protected:
      // Sets the name and help text in the MetricFamily.
      void collect_internal(MetricFamily* mf) const;

      // These static methods wrap operations on protobuf objects so
      // the header only needs a forward declaration of the classes.
      static client::Metric* add_metric(client::MetricFamily* mf);
      static client::LabelPair* add_label(client::Metric* m);
      static void set_label(client::LabelPair* l, std::string const& name,
                            std::string const& value);

      std::string name_;
      std::string help_;
    };

    template <int N, class ValueType>
    class Metric : public AbstractMetric {
      // A labeled metric has 1 or more labels. It contains a
      // collection of ValueType objects, indexed by the set of label
      // values that represent this value.

      // Convenience typedefs.
      typedef std::array<std::string, N> stringarray;
      typedef std::unordered_map<stringarray, ValueType,
                                 util::ContainerHash<stringarray>,
                                 util::ContainerEq<stringarray>> map;

     public:
      // This constructor allows specifying a custom collector. It's
      // commented out because it causes issues in the template
      // inference of gcc.
      //
      // template <typename... ValueArgs>
      // Metric(std::string const& name, std::string const& help,
      //               stringarray const& labelnames, ValueArgs const&... va) :
      //   Metric(name, help, labelnames, global_collector, va...) {}

      // A Metric is constructed with a name, help text, and a
      // set of label names, as well as any arguments required by the
      // ValueType.
      // At construction time, a "master" ValueType is created and any
      // new instance created in calls to labels() will be copied from
      // this ValueType. This allows us to do expensive one-off
      // initialization only once.
      template <typename... ValueArgs>
      Metric(std::string const& name, std::string const& help,
                    stringarray const& labelnames, ValueArgs&&... va)
          : AbstractMetric(name, help, &global_collector),
            default_value_(std::forward<ValueArgs>(va)...),
            labelnames_(labelnames) {
        static_assert(N >= 1, "A Metric should have at least 1 label.");
        const std::regex label_name_re("^[a-zA-Z_:][a-zA-Z0-9_:]*$");
        for (auto const& l : labelnames_) {
          if (l == "le" || l == "quantile" ||
              !std::regex_match(l, label_name_re)) {
            throw err::InvalidNameException();
          }
        }
      }

      // Allow construction from rvalues to facilitate:
      // auto c = makeCounter(...);
      Metric(Metric&& other) :
        AbstractMetric(other),
        default_value_(std::move(other.default_value_)),
        labelnames_(std::move(other.labelnames_)),
        mutex_()
      {}

      // Returns the ValueType instance indexed by the set of label
      // values passed. The ValueType instance is created if needed.
      ValueType& labels(stringarray const& labelvalues) {
        std::unique_lock<std::mutex> l(mutex_);
        return (values_.insert(typename map::value_type(
                    labelvalues, default_value_))).first->second;
      }

      template <typename... Args>
      ValueType&
      labels(Args... args) {
        static_assert(sizeof...(Args) == N, "label count mismatch");
        return labels({args...});
      }

      // Removes a given set of label values and the instance of
      // ValueType it references. No-op if this set of label values
      // did not reference a ValueType instance yet. This invalidates
      // any previously returned ValueType reference to the ValueType
      // instance referred to by this set of labelvalues.
      void remove(stringarray const& labelvalues) {
        std::unique_lock<std::mutex> l(mutex_);
        values_.erase(labelvalues);
      }

      // Removes all sets of label values and their corresponding
      // instance of ValueType. This invalidates any previously
      // returned ValueType reference.
      void clear() {
        std::unique_lock<std::mutex> l(mutex_);
        values_.clear();
      }

      // Collects all values in this metric to a protobuf
      // MetricFamily.
      virtual void collect(MetricFamily* mf) const {
        collect_internal(mf);
        ValueType::set_metricfamily_type(mf);
        std::unique_lock<std::mutex> l(mutex_);
        for (const auto& it_v : values_) {
          client::Metric* m = add_metric(mf);
          auto it_labelname = labelnames_.begin();
          auto it_labelvalue = it_v.first.begin();
          while (it_labelname != labelnames_.end()) {
            client::LabelPair* l = add_label(m);
            set_label(l, *it_labelname, *it_labelvalue);
            ++it_labelname;
            ++it_labelvalue;
          }
          it_v.second.collect_value(m);
        }
      }

     private:
      ValueType default_value_;
      stringarray const labelnames_;
      mutable std::mutex mutex_;
      map values_;
    };

    template <class ValueType>
    class Metric<0, ValueType> : public AbstractMetric, public ValueType {
      // An unlabeled metric contains one and only one instance of the
      // ValueType. For convenience, we model this through inheritance
      // so it is possible to call the methods of the ValueType
      // directly on the metric.

     public:
      // This constructor allows specifying a custom collector. It's
      // commented out because it causes issues in the template
      // inference of gcc.
      //
      // template <typename... ValueArgs>
      // Metric(std::string const& name, std::string const& help,
      //                 ValueArgs const&... va)
      //   : Metric(name, help, global_collector, va...)
      // {}

      template <typename... ValueArgs>
      Metric(std::string const& name, std::string const& help,
                      ValueArgs&&... va)
          : AbstractMetric(name, help, &global_collector),
            ValueType(std::forward<ValueArgs>(va)...) {}

      // Collects the metric and its value to a MetricFamily protobuf.
      virtual void collect(MetricFamily* mf) const {
        collect_internal(mf);
        ValueType::set_metricfamily_type(mf);
        this->collect_value(add_metric(mf));
      }
    };

  } /* namespace impl */
} /* namespace prometheus */

#endif
