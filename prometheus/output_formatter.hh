#ifndef PROMETHEUS_OUTPUT_FORMATTER_HH__
#define PROMETHEUS_OUTPUT_FORMATTER_HH__

#include "prometheus/proto/metrics.pb.h"

#include <string>

namespace prometheus {

  using ::prometheus::client::MetricFamily;

  // Converts from the protobuf exposition format to the text
  // exposition format.
  void metricfamily_proto_to_ostream(
    std::ostream& os, MetricFamily const* mf);
  std::string metricfamily_proto_to_string(MetricFamily const* mf);

  // Escaping functions for each element of the text format.
  std::string escape_double(double d);
  std::string escape_metric_name(std::string const& s);
  std::string escape_help(std::string const& s);
  std::string escape_label_name(std::string const& s);
  std::string escape_label_value(std::string const& s);

  namespace impl {

    // This exception is raised if the provided protobuf can't be
    // converted to the text format.
    class OutputFormatterException : public std::exception {
    public:
      explicit OutputFormatterException(const char* reason) noexcept
	: reason_(reason) {}
      virtual const char* what() const noexcept { return reason_; }

      static const char* const kEmptyMetricFamily;
      static const char* const kInvalidMetricType;
      static const char* const kMissingRequiredField;
      static const char* const kSummariesNotImplemented;

    private:
      const char* reason_;
    };

  } /* namespace impl */
} /* namespace prometheus */

#endif
