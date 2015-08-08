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

  // Sets the gauge to the current Unix timestamp in floating-point
  // seconds. Precision and epoch are dependent on the provided
  // clock. Note that when using a custon duration_t, the durations
  // provided in std::chrono (e.g. std::chrono::seconds) don't use
  // floating point precision internally. Make your own durations if
  // you want floatint point precision.
  template<typename clock_t = std::chrono::system_clock,
	   typename duration_t = std::chrono::duration<
	     double, std::chrono::seconds::period>>
    void set_to_current_time(impl::SetGaugeValue& gauge) {
    gauge.set(std::chrono::duration_cast<duration_t>(
		clock_t::now().time_since_epoch()).count());
  }

  template <typename clock_t, typename duration_t>
  class AbstractIntervalMeasure {
    // This is the basis of a simple RAII-based class that measures
    // the time elapsed between its construction and its destruction
    // and reports it into a Histogram (via IntervalAccumulator) or a
    // SetGauge (via IntervalReporter).

    // Histogram<0> long_comp_hist(
    //            "long_computation_time_seconds",
    //            "Histogram of time spent in each long computation")
    // void run_long_computation() {
    //   IntervalAccumulator ia(long_comp_hist);
    //   // Do something complicated here.
    // }

  public:
    AbstractIntervalMeasure()
      : begin_(clock_t::now()) {
      static_assert(std::chrono::treat_as_floating_point<
		    typename duration_t::rep>::value,
		    "The duration passed to AbstractIntervalMeasure must be "
		    "expressed in a floating point type, ideally double.");
    }

    double value() {
      return (std::chrono::duration_cast<duration_t>(
		clock_t::now() - begin_).count());
    }

  private:
    AbstractIntervalMeasure(AbstractIntervalMeasure const&) = delete;
    AbstractIntervalMeasure(AbstractIntervalMeasure&&) = delete;
    AbstractIntervalMeasure& operator=(AbstractIntervalMeasure const&) = delete;
    AbstractIntervalMeasure& operator=(AbstractIntervalMeasure&&) = delete;
    std::chrono::time_point<clock_t> begin_;
  };


  template <
    typename clock_t = std::chrono::steady_clock,
    typename duration_t = std::chrono::duration<
      double, std::chrono::seconds::period> >
  class IntervalAccumulator : AbstractIntervalMeasure<clock_t, duration_t> {
    // See Abstractintervalmeasure for information.
    // Usage:
    //
    // Histogram<0> long_comp_hist(
    //            "long_computation_time_seconds",
    //            "Histogram of time spent in each long computation")
    // void run_long_computation() {
    //   IntervalAccumulator<> ia(long_comp_hist);
    //   // Do something complicated here.
    // }

  public:
    IntervalAccumulator(impl::HistogramValue& h)
      : h_(h) {}

    ~IntervalAccumulator() {
      h_.observe(this->value());
    }

  private:
    impl::HistogramValue& h_;
  };

  template <
    typename clock_t = std::chrono::steady_clock,
    typename duration_t = std::chrono::duration<
      double, std::chrono::seconds::period> >
  class IntervalReporter : AbstractIntervalMeasure<clock_t, duration_t> {
    // See Abstractintervalmeasure for information.
    // Usage:
    //
    // SetGauge<0> long_comp_last(
    //            "last_long_computation_time_seconds",
    //            "Time spent in the last long computation")
    // void run_long_computation() {
    //   IntervalReporter<> ir(long_comp_last);
    //   // Do something complicated here.
    // }

  public:
    IntervalReporter(impl::SetGaugeValue& g)
      : g_(g) {}

    ~IntervalReporter() {
      g_.set(this->value());
    }

  private:
    impl::SetGaugeValue& g_;
  };
}

#endif
