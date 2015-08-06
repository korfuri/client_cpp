#ifndef PROMETHEUS_UTILS_HH__
#define PROMETHEUS_UTILS_HH__

#include "values.hh"

#include <chrono>

namespace prometheus {

  class InProgress {
   public:
    InProgress(impl::IncDecGaugeValue& g, double value = 1.0);
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
    typename duration_t = std::chrono::milliseconds>
  class IntervalAccumulator {
   public:
    IntervalAccumulator(impl::HistogramValue& h)
        : h_(h), begin_(clock_t::now()) {}

    ~IntervalAccumulator() {
      h_.record(std::chrono::duration_cast<duration_t>(
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
