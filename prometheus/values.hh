#ifndef PROMETHEUS_VALUES_HH__
#define PROMETHEUS_VALUES_HH__

#include "proto/stubs.hh"

#include <atomic>
#include <mutex>
#include <string>
#include <vector>

namespace prometheus {

  // +Inf. For convenience.
  extern const double kInf;

  // The set of buckets used by default in histograms.
  extern const std::vector<double> default_histogram_levels;


  // This wrapper ensures that histogram levels are not confused with
  // label name sets or other arguments when building LabeledMetrics.
  std::vector<double> histogram_levels(std::vector<double>&&);

  // Constructs a list of `count` histogram buckets increasing
  // exponentially, from `base**starting_exponent`. 0 and +Inf are
  // added at the beginning and end, respectively. Throws a
  // logic_error if count or base is <= 0.
  std::vector<double> histogram_levels_powers_of(double base, int count,
						 double starting_exponent = 0);

  // Constructs a list of `count` histogram buckets increasing
  // linearly by `increment`, starting from `start`. +Inf is added at
  // end automatically. Throws a logic_error if count or increment is
  // <= 0.
  std::vector<double> histogram_levels_linear(double start, int count,
					      double increment = 1.0);

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
      // A base class used by the various types of gauges. We separate
      // "Inc/Dec" gauges from "Set" gauges to allow us to squeeze
      // some performance by using different types of memory barriers.
     public:
      void collect_value(Metric* m, MetricFamily* mf) const;
    };

    class SetGaugeValue : public BaseGaugeValue {
      // A gauge that can be set to a given value.
     public:
      // Sets the gauge to `value`.
      void set(double value) { value_.store(value, std::memory_order_relaxed); }
    };

    class IncDecGaugeValue : public BaseGaugeValue {
      // A gauge that can only be incremented or decremented.
     public:
      void inc(double value = 1.0);
      void dec(double value = 1.0);
    };

    class CounterValue : public BaseScalarValue {
      // A counter is a scalar value that can only be
      // incremented. Decrementing a counter throws a
      // NegativeCounterIncrementException.
     public:
      void inc(double value = 1.0);
      void collect_value(Metric* m, MetricFamily* mf) const;
    };

    class HistogramValue {
      // This is the internal representation of a histogram.

     public:
      // A histogram is always constructed with a fixed list of
      // strictly increasing levels.
      HistogramValue(
          std::vector<double> const& levels = default_histogram_levels);
      ~HistogramValue();
      HistogramValue(HistogramValue const& rhs);

      // Observe the given value. This increments all buckets whose
      // threshold is superior or equal to the value.
      void observe(double value);

      // Returns true if d is +Inf.
      static bool is_posinf(double d);
      // Adds a +Inf bucket to the vector if none was added.
      static std::vector<double> add_inf(std::vector<double> const&);

      // Returns the count of observed events at the given threshold
      // (rounded up to the next bucket if necessary). This is useful
      // for testing. Defaults to +Inf, which means returning the
      // total count of observed values.
      double value(double threshold = kInf) const;

      // Collects the value to a MetricFamily.
      void collect_value(Metric* m, MetricFamily* mf) const;

     private:
      mutable std::mutex mutex_;
      // TODO(korfuri): The current usage duplicates the list of
      // levels for each HistogramValue, causing unnecessary memory
      // usage in labeled histograms. We should store the levels in
      // the Metric or in an intermediary class, to avoid duplication.
      const std::vector<double> levels_;
      std::vector<uint64_t> values_;
      double samples_sum_;
    };

  } /* namespace impl */
} /* namespace prometheus */

#endif
