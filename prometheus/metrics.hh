#ifndef PROMETHEUS_METRICS_HH__
#define PROMETHEUS_METRICS_HH__

#include "output_formatter.hh"
#include "prometheus/proto/metrics.pb.h"
#include "util/container_hash.hh"
#include "util/zipped_iterator.hh"

#include <algorithm>
#include <array>
#include <mutex>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace prometheus {
  namespace impl {

    using ::io::prometheus::client::LabelPair;
    using ::io::prometheus::client::MetricFamily;
    using ::io::prometheus::client::Metric;

    class Registry;

    class AbstractMetric {
     public:
      AbstractMetric(const std::string &name, const std::string &help);
      AbstractMetric(const std::string &name, const std::string &help,
                     Registry *reg);
      virtual void output(OutputFormatter &) const = 0;
      virtual void output_proto(MetricFamily* mf) const = 0;

     protected:
      void output_proto_internal(MetricFamily* mf) const;
      std::string name_;
      std::string help_;
    };

    template <int N, class ValueType>
    class LabeledMetric : public AbstractMetric {
      typedef std::array<std::string, N> stringarray;
      typedef std::unordered_map<stringarray, ValueType,
				 util::ContainerHash<stringarray>,
				 util::ContainerEq<stringarray>> map;

     public:
      template<typename... ValueArgs>
      LabeledMetric(std::string const &name, std::string const &help,
                    stringarray const &labelnames, ValueArgs const&... va)
	: AbstractMetric(name, help), default_value_(va...), labelnames_(labelnames) {
        static_assert(N >= 1, "A LabeledMetric should have at least 1 label.");
      }

      ValueType &labels(stringarray const &labelvalues) {
        std::unique_lock<std::mutex> l(mutex_);
        return (values_.insert(typename map::value_type(labelvalues, default_value_))).first->second;
      }

      virtual void output(OutputFormatter &f) const {
        f.addMetric(name_, help_, ValueType::type_);
        std::unique_lock<std::mutex> l(mutex_);
        for (const auto &it_v : values_) {
          auto zbegin =
              util::zip_iterators(labelnames_.begin(), it_v.first.begin());
          auto zend = util::zip_iterators(labelnames_.end(), it_v.first.end());
	  it_v.second.value_output(f, name_, zbegin, zend);
	}
      }

      virtual void output_proto(MetricFamily* mf) const {
	output_proto_internal(mf);
        std::unique_lock<std::mutex> l(mutex_);
        for (const auto &it_v : values_) {
	  Metric* m = mf->add_metric();
	  auto it_labelname = labelnames_.begin();
	  auto it_labelvalue = it_v.first.begin();
	  while (it_labelname != labelnames_.end()) {
	    LabelPair* l = m->add_label();
	    l->set_name(*it_labelname);
	    l->set_value(*it_labelvalue);
	    ++it_labelname;
	    ++it_labelvalue;
	  }
	  it_v.second.output_proto_value(m, mf);
	}
      }

     private:
      ValueType default_value_;
      stringarray const labelnames_;
      mutable std::mutex mutex_;
      map values_;
    };

    template <class ValueType>
    class UnlabeledMetric : public AbstractMetric, public ValueType {
     public:
      template <typename... ValueArgs>
      UnlabeledMetric(std::string const &name, std::string const &help,
		      ValueArgs const&... va)
	: AbstractMetric(name, help), ValueType(va...) {}

      virtual void output(OutputFormatter &f) const {
        f.addMetric(name_, help_, ValueType::type_);
	this->value_output(f, name_);
      }

      virtual void output_proto(MetricFamily* mf) const {
	output_proto_internal(mf);
	this->output_proto_value(mf->add_metric(), mf);
      }
    };

  } /* namespace impl */
} /* namespace prometheus */

#endif
