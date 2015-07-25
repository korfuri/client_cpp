#include "values.hh"

namespace prometheus {
namespace impl {

void CounterValue::inc(double value) {
  // if (value < 0) throw Something();
  double oldv, newv;
  do {
    oldv = value_.load(std::memory_order_acquire);
    newv = oldv + value;
  } while (value_.exchange(newv) != oldv);
}
 
const std::string CounterValue::type_ = "counter";
const std::string GaugeValue::type_ = "gauge";

}
}
