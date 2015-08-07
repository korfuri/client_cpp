#include "exceptions.hh"
#include "values.hh"
#include "prometheus/proto/metrics.pb.h"

#include <cmath>
#include <stdexcept>
#include <limits>
#include <mutex>
#include <utility>
#include <vector>

namespace prometheus {

  const double kInf = std::numeric_limits<double>::infinity();

  std::vector<double> histogram_levels(std::vector<double>&& v) {
    return std::move(v);
  }

  const std::vector<double> default_histogram_levels =
      histogram_levels({.005, .01, .025, .05, .075, .1, .25, .5, .75, 1.0, 2.5,
                        5.0, 7.5, 10.0, kInf});

  std::vector<double> histogram_levels_powers_of(double base, int count,
						 double starting_exponent) {
    std::vector<double> v(count + 2);
    if (count <= 0) {
      throw std::logic_error("invalid count of histogram buckets");
    }
    if (base <= 0) {
      throw std::logic_error("base must be a positive number");
    }
    double exp = starting_exponent;
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

    void BaseGaugeValue::collect_value(Metric* m, MetricFamily* mf) const {
      m->mutable_gauge()->set_value(value_);
      mf->set_type(::io::prometheus::client::MetricType::GAUGE);
    }

    void IncDecGaugeValue::inc(double value) {
      double current = value_.load();
      while (!(value_.compare_exchange_weak(current, current + value)))
        ;
    }

    void IncDecGaugeValue::dec(double value) { inc(-value); }

    void CounterValue::inc(double value) {
      if (value < 0) {
        throw err::NegativeCounterIncrementException();
      }
      double current = value_.load();
      while (!(value_.compare_exchange_weak(current, current + value)))
        ;
    }

    void CounterValue::collect_value(Metric* m, MetricFamily* mf) const {
      m->mutable_counter()->set_value(value_);
      mf->set_type(::io::prometheus::client::MetricType::COUNTER);
    }

    HistogramValue::HistogramValue(std::vector<double> const& levels)
        : levels_(add_inf(levels)), values_(levels_.size()) {
      double last_level = std::numeric_limits<double>::lowest();
      for (auto const& l : levels) {
        if (l <= last_level) {
          throw err::UnsortedLevelsException();
        }
        last_level = l;
      }
    }

    HistogramValue::HistogramValue(HistogramValue const& rhs)
        : levels_(rhs.levels_), values_(rhs.levels_.size()) {}

    HistogramValue::~HistogramValue() {}

    /* static */ bool HistogramValue::is_posinf(double d) {
      return std::isinf(d) && d > 0;
    }

    /* static */ std::vector<double> HistogramValue::add_inf(
        std::vector<double> const& in) {
      if (is_posinf(in.back())) {
        return in;
      } else {
        std::vector<double> out(in.size() + 1);
        std::copy(in.begin(), in.end(), out.begin());
        out.back() = std::numeric_limits<double>::infinity();
        return out;
      }
    }

    void HistogramValue::observe(double v) {
      auto lvl_it = levels_.begin();
      auto v_it = values_.begin();
      {
        std::lock_guard<std::mutex> l(mutex_);
        while (lvl_it != levels_.end()) {
          if (v <= *lvl_it) {
            *v_it += 1;
          }
          ++lvl_it;
          ++v_it;
        }
        samples_sum_ += v;
      }
    }

    double HistogramValue::value(double d) const {
      auto lvl_it = levels_.begin();
      auto v_it = values_.begin();
      {
        std::lock_guard<std::mutex> l(mutex_);
        while (lvl_it != levels_.end()) {
          if (d <= *lvl_it) {
            return *v_it;
          }
          ++lvl_it;
          ++v_it;
        }
        return values_.back();
      }
    }

    void HistogramValue::collect_value(Metric* m, MetricFamily* mf) const {
      Histogram* h = m->mutable_histogram();
      mf->set_type(::io::prometheus::client::MetricType::HISTOGRAM);
      auto it = values_.begin();
      {
        std::lock_guard<std::mutex> l(mutex_);
        h->set_sample_count(values_.back());
        h->set_sample_sum(samples_sum_);
        for (auto const& l : levels_) {
          Bucket* b = h->add_bucket();
          b->set_upper_bound(l);
          b->set_cumulative_count(*it);
          ++it;
        }
      }
    }
  }
}
