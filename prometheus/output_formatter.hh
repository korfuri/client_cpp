#ifndef PROMETHEUS_OUTPUT_FORMATTER_HH__
#define PROMETHEUS_OUTPUT_FORMATTER_HH__

#include <string>

#include "family.hh"
#include "prometheus/proto/metrics.pb.h"

namespace prometheus {

  enum exposition_format {
    fmt_text,
    fmt_proto,             // protobuf delimited
    //fmt_proto_txt        // not implemented
    //fmt_proto_compact,   // not implemented
  };

  std::string
  render(collection_type const& collection, exposition_format format);

  void
  render(collection_type const& collection, exposition_format format,
      std::ostream & os);

  std::string
  collection_to_text(collection_type const& collection);

  void
  collection_to_text(collection_type const& collection, std::ostream & os);

  std::string
  collection_to_proto_delimited(collection_type const& collection);

  void
  collection_to_proto_delimited(collection_type const& collection, std::ostream & os);

  // Converts from the protobuf exposition format to the text
  // exposition format.
  void metricfamily_proto_to_ostream(std::ostream& os, MetricFamilyPtr mf);
  std::string metricfamily_proto_to_string(MetricFamilyPtr mf);

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

      static const char* const kInvalidMetricType;
      static const char* const kMissingRequiredField;
      static const char* const kSummariesNotImplemented;

    private:
      const char* reason_;
    };

  } /* namespace impl */
} /* namespace prometheus */

#endif
