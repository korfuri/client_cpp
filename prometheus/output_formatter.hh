#ifndef PROMETHEUS_OUTPUT_FORMATTER_HH__
#define PROMETHEUS_OUTPUT_FORMATTER_HH__

#include <ostream>

#include "prometheus/proto/metrics.pb.h"

namespace prometheus {

  namespace impl {

    class OutputFormatter {
     public:
      OutputFormatter(std::ostream &os);

      void addMetric(std::string const &name, std::string const &help,
                     std::string const &type);

      void addMetricValue(double value, std::string const &name);
      void addMetricValue(double value, double bucket, std::string const &name);

      template <typename LabelIterator>
      void addMetricValue(double value, std::string const &name,
			  LabelIterator const &labels_begin,
			  LabelIterator const &labels_end) {
        if (labels_begin == labels_end) {
          os_ << escape_metric_name(name) << " = " << escape_double(value) << std::endl;
        } else {
          os_ << escape_metric_name(name);
          char nextchar = '{';
          for (auto it = labels_begin; it != labels_end; ++it) {
            os_ << nextchar << escape_label_name((*it).first) << '=' << escape_label_value((*it).second);
            nextchar = ',';
          }
          os_ << "} = " << escape_double(value) << std::endl;
        }
      }

      template <typename LabelIterator>
      void addMetricValue(double value, double bucket,
			  std::string const &name,
			  LabelIterator const &labels_begin,
			  LabelIterator const &labels_end) {
	os_ << escape_metric_name(name) << "{le=" << escape_double(bucket);
	for (auto it = labels_begin; it != labels_end; ++it) {
	  os_ << ',' << escape_label_name((*it).first) << '=' << escape_label_value((*it).second);
	}
	os_ << "} = " << escape_double(value) << std::endl;
      }

      std::string escape_double(double d);
      std::string escape_metric_name(std::string const& s);
      std::string escape_help(std::string const& s);
      std::string escape_label_name(std::string const& s);
      std::string escape_label_value(std::string const& s);

     private:
      std::ostream &os_;
    };
  }
}

#endif
