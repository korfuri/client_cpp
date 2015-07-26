#ifndef PROMETHEUS_VALUES_HH__
#define PROMETHEUS_VALUES_HH__

#include <atomic>
#include <string>

namespace prometheus {
namespace impl {

class BaseScalarValue {
 public:
  BaseScalarValue() : value_(0) {}
  ~BaseScalarValue() {}

  double value() const { return value_.load(std::memory_order_relaxed); }

 private:
  BaseScalarValue(BaseScalarValue const&) = delete;
  BaseScalarValue(BaseScalarValue&) = delete;
  BaseScalarValue& operator=(BaseScalarValue const&) = delete;
  BaseScalarValue& operator=(BaseScalarValue&&) = delete;

 protected:
  std::atomic<double> value_;
};

class GaugeValue : public BaseScalarValue {
 public:
  void set(double value) { value_.store(value, std::memory_order_relaxed); }

  const static std::string type_;
};

class CounterValue : public BaseScalarValue {
 public:
  void inc(double value = 1.0);

  const static std::string type_;
};

class HistogramValue : public BaseScalarValue {
 public:
  void inc();

  const static std::string type_;
};

} /* namespace impl */
} /* namespace prometheus */

#endif
