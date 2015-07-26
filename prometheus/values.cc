#include "values.hh"

namespace prometheus {
namespace impl {

void CounterValue::inc(double value) {
  double current = value_.load();
  while (!(value_.compare_exchange_weak(current, current + value)))
    ;
}

const std::string CounterValue::type_ = "counter";
const std::string GaugeValue::type_ = "gauge";
}
}
