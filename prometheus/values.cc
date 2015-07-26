#include "exceptions.hh"
#include "values.hh"

namespace prometheus {
  namespace impl {

    void IncDecGaugeValue::inc(double value) {
      double current = value_.load();
      while (!(value_.compare_exchange_weak(current, current + value)))
        ;
    }

    void IncDecGaugeValue::dec(double value) {
      inc(-value);
    }

    void CounterValue::inc(double value) {
      if (value < 0) {
        throw err::NegativeCounterIncrementException();
      }
      double current = value_.load();
      while (!(value_.compare_exchange_weak(current, current + value)))
        ;
    }

    void HistogramValue::inc() {
      double current = value_.load();
      while (!(value_.compare_exchange_weak(current, current + 1.0)))
        ;
    }

    const std::string CounterValue::type_ = "counter";
    const std::string SetGaugeValue::type_ = "gauge";
    const std::string IncDecGaugeValue::type_ = "gauge";
    const std::string HistogramValue::type_ = "histogram";
  }
}
