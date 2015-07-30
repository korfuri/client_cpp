#include "exceptions.hh"
#include "values.hh"

#include <cmath>
#include <stdexcept>
#include <limits>
#include <utility>
#include <vector>

namespace prometheus {

  const double kInf = std::numeric_limits<double>::infinity();

  std::vector<double> histogram_levels(std::vector<double>&& v) {
    return std::move(v);
  }

  const std::vector<double> default_histogram_levels = histogram_levels({
      .005, .01, .025, .05, .075, .1, .25, .5, .75, 1.0, 2.5, 5.0, 7.5, 10.0, kInf});

  std::vector<double> histogram_levels_powers_of(double base, double count) {
    std::vector<double> v(count + 2);
    if (count <= 0) {
      throw std::logic_error("invalid count of histogram buckets");
    }
    double exp = 0.0;
    v[0] = 0.0;
    for (auto it = v.begin() + 1; it != v.end(); ++it) {
      *it = pow(base, exp);
      exp += 1.0;
    }
    v.back() = kInf;
    return v;
  }

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

    HistogramValue::HistogramValue(std::vector<double> const& levels) :
      levels_(add_inf(levels)),
      values_(new std::atomic<double>[levels_.size()]) {
      double last_level = std::numeric_limits<double>::lowest();
      for (auto const &l : levels) {
        if (l <= last_level) {
          throw err::UnsortedLevelsException();
        }
        last_level = l;
      }
    }

    HistogramValue::HistogramValue(HistogramValue const& rhs) :
      levels_(rhs.levels_),
      values_(new std::atomic<double>[rhs.levels_.size()]) {
    }


    HistogramValue::~HistogramValue() {
      delete[] values_;
    }

    bool HistogramValue::is_posinf(double d) {
      return std::isinf(d) && d > 0;
    }

    std::vector<double> HistogramValue::add_inf(std::vector<double> const& in) {
      if (is_posinf(in.back())) {
	return in;
      } else {
	std::vector<double> out(in.size() + 1);
	std::copy(in.begin(), in.end(), out.begin());
	out.back() = std::numeric_limits<double>::infinity();
	return out;
      }
    }

    void HistogramValue::record(double v) {
      auto lvl_it = levels_.begin();
      auto v_it = &(values_[0]);
      while (lvl_it != levels_.end()) {
	if (v <= *lvl_it) {
	  double current = v_it->load();
	  while (!(v_it->compare_exchange_weak(current, current + 1.0)))
	    ;
	}
	++lvl_it;
	++v_it;
      }
    }

    double HistogramValue::value(double d) const {
      auto lvl_it = levels_.begin();
      auto v_it = &(values_[0]);
      while (lvl_it != levels_.end()) {
	if (d <= *lvl_it) {
	  return v_it->load(std::memory_order_relaxed);
	}
	++lvl_it;
	++v_it;
      }
      return values_[levels_.size() - 1].load(std::memory_order_relaxed);
    }

    const std::string CounterValue::type_ = "counter";
    const std::string SetGaugeValue::type_ = "gauge";
    const std::string IncDecGaugeValue::type_ = "gauge";
    const std::string HistogramValue::type_ = "histogram";
  }
}
