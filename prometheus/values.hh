#ifndef PROMETHEUS_VALUES_HH__
#define PROMETHEUS_VALUES_HH__

#include "prometheus/proto/metrics.pb.h"
#include "output_formatter.hh"

#include <atomic>
#include <mutex>
#include <string>
#include <vector>

namespace prometheus {

  extern const double kInf;
  extern const std::vector<double> default_histogram_levels;

  std::vector<double> histogram_levels(std::vector<double>&&);
  std::vector<double> histogram_levels_powers_of(double base, double count);

  namespace impl {

    using ::io::prometheus::client::Metric;
    using ::io::prometheus::client::MetricFamily;

    class BaseScalarValue {
      // A base class used by the various scalar values (counter and gauges).
     public:
      BaseScalarValue() : value_(0) {
        static_assert(sizeof(BaseScalarValue) == sizeof(std::atomic<double>),
                      "BaseScalarValue is not meant to have a vtable or to be "
                      "used polymorphically.");
      }
      ~BaseScalarValue() {}
      BaseScalarValue(BaseScalarValue const& rhs) : value_(rhs.value_.load()) {}

      double value() const { return value_.load(std::memory_order_relaxed); }

     protected:
      std::atomic<double> value_;
    };

    class BaseGaugeValue : public BaseScalarValue {
     public:
      void output_proto_value(Metric* m, MetricFamily* mf) const;

      const static std::string type_;
    };

    class SetGaugeValue : public BaseGaugeValue {
     public:
      void set(double value) { value_.store(value, std::memory_order_relaxed); }
    };

    class IncDecGaugeValue : public BaseGaugeValue {
     public:
      void inc(double value = 1.0);
      void dec(double value = 1.0);
    };

    class CounterValue : public BaseScalarValue {
     public:
      void inc(double value = 1.0);
      void output_proto_value(Metric* m, MetricFamily* mf) const;

      const static std::string type_;
    };

    class HistogramValue {
     public:
      HistogramValue(
          std::vector<double> const& levels = default_histogram_levels);
      ~HistogramValue();
      HistogramValue(HistogramValue const& rhs);
      void record(double value);

      static bool is_posinf(double d);
      static std::vector<double> add_inf(std::vector<double> const&);

      double value(double threshold = kInf) const;

      void output_proto_value(Metric* m, MetricFamily* mf) const;

      const static std::string type_;

     private:
      mutable std::mutex mutex_;
      const std::vector<double> levels_;
      std::vector<uint64_t> values_;
      double samples_sum_;
    };

  } /* namespace impl */
} /* namespace prometheus */

#endif
