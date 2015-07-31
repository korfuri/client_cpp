#ifndef PROMETHEUS_VALUES_HH__
#define PROMETHEUS_VALUES_HH__

#include "output_formatter.hh"

#include <atomic>
#include <string>
#include <vector>

namespace prometheus {

  extern const double kInf;
  extern const std::vector<double> default_histogram_levels;

  std::vector<double> histogram_levels(std::vector<double>&&);
  std::vector<double> histogram_levels_powers_of(double base, double count);

  namespace impl {

    class BaseScalarValue {
      // A base class used by the various scalar values (counter and gauges).
     public:
      BaseScalarValue() : value_(0) {}
      ~BaseScalarValue() {}
      BaseScalarValue(BaseScalarValue const& rhs) : value_(rhs.value_.load()) {}

      double value() const { return value_.load(std::memory_order_relaxed); }

      template<typename... ContextParams>
      void value_output(OutputFormatter& f, ContextParams const&... params) const {
        f.addMetricValue(this->value_, params...);
      }

     protected:
      std::atomic<double> value_;
    };

    class SetGaugeValue : public BaseScalarValue {
     public:
      void set(double value) { value_.store(value, std::memory_order_relaxed); }

      const static std::string type_;
    };

    class IncDecGaugeValue : public BaseScalarValue {
     public:
      void inc(double value = 1.0);
      void dec(double value = 1.0);

      const static std::string type_;
    };

    class CounterValue : public BaseScalarValue {
     public:
      void inc(double value = 1.0);

      const static std::string type_;
    };

    class HistogramValue {
    public:
      HistogramValue(std::vector<double> const& levels = default_histogram_levels);
      ~HistogramValue();
      HistogramValue(HistogramValue const& rhs);
      void record(double value);

      static bool is_posinf(double d);
      static std::vector<double> add_inf(std::vector<double> const&);

      double value(double threshold = kInf) const;

      template<typename... ContextParams>
      void value_output(OutputFormatter& f, ContextParams const&... params) const {
	auto it = values_.begin();
	for (auto const& l : levels_) {
	  f.addMetricValue(*it, l, params...);
	  ++it;
	}
      }

      const static std::string type_;

    private:
      const std::vector<double> levels_;
      std::vector<std::atomic<double>> values_;
    };

  } /* namespace impl */
} /* namespace prometheus */

#endif
