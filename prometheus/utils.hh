#ifndef PROMETHEUS_UTILS_HH__
#define PROMETHEUS_UTILS_HH__

#include "values.hh"

#include <chrono>

namespace prometheus {

  // Lets users build a metric name in the form "namespace_subsystem_metric".
  // This is a simple concatenation.
  std::string make_metric_name(std::string const& namespace_name,
			       std::string const& subsystem_name,
			       std::string const& metric_name);

  class InProgress {
    // This is a simple RAII-based class that increments and
    // decrements an IncDecGauge when the control flow respectively
    // enters and leaves the lifetime of thie InProgress instance.
    // You can use this easily to track how many threads are currently
    // executing a function:
    //
    // IncDecGauge<0> long_comp(
    //            "long_computation_current",
    //            "Number of threads running the long computation")
    // void run_long_computation() {
    //   InProgress ip(long_comp);
    //   // Do something complicated here.
    // }

   public:
    // The contructor increments the IncDecGauge by value.
    InProgress(impl::IncDecGaugeValue& g, double value = 1.0);
    // The destructor decrements the IncDecGauge by value.
    ~InProgress();

   private:
    InProgress(InProgress const&) = delete;
    InProgress(InProgress&&) = delete;
    InProgress& operator=(InProgress const&) = delete;
    InProgress& operator=(InProgress&&) = delete;
    impl::IncDecGaugeValue& g_;
    double value_;
  };

  template <
    typename clock_t = std::chrono::steady_clock,
    typename duration_t = std::chrono::duration<double,
						std::chrono::seconds::period>>
    class IntervalAccumulator {
    // This is a simple RAII-based class that measures the time
    // elapsed between its construction and its destruction and
    // reports it into a Histogram.
    // You can use it easily:
    //
    // Histogram<0> long_comp_hist(
    //            "long_computation_time_seconds",
    //            "Histogram of time spent in each long computation")
    // void run_long_computation() {
    //   IntervalAccumulator ia(long_comp_hist);
    //   // Do something complicated here.
    // }

   public:
    IntervalAccumulator(impl::HistogramValue& h)
        : h_(h), begin_(clock_t::now()) {
      static_assert(std::chrono::treat_as_floating_point<
		    typename duration_t::rep>::value,
		    "The duration passed to IntervalAccumulator must be "
		    "expressed in a floating point type, ideally double.");
    }

    ~IntervalAccumulator() {
      h_.observe(std::chrono::duration_cast<duration_t>(
        clock_t::now() - begin_).count());
    }

   private:
    IntervalAccumulator(IntervalAccumulator const&) = delete;
    IntervalAccumulator(IntervalAccumulator&&) = delete;
    IntervalAccumulator& operator=(IntervalAccumulator const&) = delete;
    IntervalAccumulator& operator=(IntervalAccumulator&&) = delete;
    impl::HistogramValue& h_;
    std::chrono::time_point<clock_t> begin_;
  };
}

#endif
