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

    using ::io::prometheus::client::Bucket;
    using ::io::prometheus::client::Histogram;
    using ::io::prometheus::client::Metric;
    using ::io::prometheus::client::MetricFamily;

    void BaseGaugeValue::output_proto_value(Metric* m, MetricFamily* mf) const {
      m->mutable_gauge()->set_value(value_);
      mf->set_type(::io::prometheus::client::MetricType::GAUGE);
    }

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

    void CounterValue::output_proto_value(Metric* m, MetricFamily* mf) const {
      m->mutable_gauge()->set_value(value_);
      mf->set_type(::io::prometheus::client::MetricType::COUNTER);
    }

    HistogramValue::HistogramValue(std::vector<double> const& levels) :
      levels_(add_inf(levels)),
      values_(levels_.size()) {
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
      values_(rhs.levels_.size()) {
    }

    HistogramValue::~HistogramValue() {
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
      auto v_it = values_.begin();
      while (lvl_it != levels_.end()) {
	if (v <= *lvl_it) {
	  uint64_t current = v_it->load();
	  while (!(v_it->compare_exchange_weak(current, current + 1)))
	    ;
	}
	++lvl_it;
	++v_it;
      }
      {
	double current = samples_sum_.load();
	while (!(samples_sum_.compare_exchange_weak(current, current + v)))
	  ;
      }
    }

    double HistogramValue::value(double d) const {
      auto lvl_it = levels_.begin();
      auto v_it = values_.begin();
      while (lvl_it != levels_.end()) {
	if (d <= *lvl_it) {
	  return v_it->load(std::memory_order_relaxed);
	}
	++lvl_it;
	++v_it;
      }
      return values_.back().load(std::memory_order_relaxed);
    }

    void HistogramValue::output_proto_value(Metric* m, MetricFamily* mf) const {
      Histogram* h = m->mutable_histogram();
      h->set_sample_count(values_.back().load());
      h->set_sample_sum(samples_sum_);
      mf->set_type(::io::prometheus::client::MetricType::HISTOGRAM);
	auto it = values_.begin();
	for (auto const& l : levels_) {
	  Bucket* b = h->add_bucket();
	  b->set_upper_bound(l);
	  b->set_cumulative_count(it->load());
	  ++it;
	}
    }

    const std::string CounterValue::type_ = "counter";
    const std::string BaseGaugeValue::type_ = "gauge";
    const std::string HistogramValue::type_ = "histogram";
  }
}
