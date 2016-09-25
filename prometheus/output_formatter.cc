#include "prometheus/proto/metrics.pb.h"
#include "output_formatter.hh"

#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <unicode/unistr.h>

namespace prometheus {

  using ::prometheus::client::Bucket;
  using ::prometheus::client::Histogram;
  using ::prometheus::client::LabelPair;
  using ::prometheus::client::Metric;
  using ::prometheus::client::MetricType;

  static std::string escape_type(MetricType const& t) {
    switch (t) {
    case MetricType::COUNTER:
      return "counter";
    case MetricType::GAUGE:
      return "gauge";
    case MetricType::SUMMARY:
      return "summary";
    case MetricType::HISTOGRAM:
      return "histogram";
    case MetricType::UNTYPED:
      return "untyped";
    default:
      throw impl::OutputFormatterException(
	impl::OutputFormatterException::kInvalidMetricType);
    }
  }

  void metric_labels_proto_to_ostream(Metric const& m, std::ostream& ss) {
    for (int i = 0; i < m.label_size(); ++i) {
      LabelPair const& lp = m.label(i);
      ss << escape_label_name(lp.name()) << '='
	 << escape_label_value(lp.value());
      if (i + 1 < m.label_size()) {
	ss << ',';
      };
    }
  }

  void metric_proto_to_ostream_common(std::string const& escaped_name,
				      Metric const& m, std::ostream& ss) {
    ss << escaped_name;
    if (m.label_size() > 0) {
      ss << '{';
      metric_labels_proto_to_ostream(m, ss);
      ss << '}';
    }
    ss << " ";
  }

  void counter_proto_to_ostream(std::string const& escaped_name,
				Metric const& m, std::ostream& ss) {
    if (!m.has_counter() || !m.counter().has_value()) {
      throw impl::OutputFormatterException(
	impl::OutputFormatterException::kMissingRequiredField);
    }
    metric_proto_to_ostream_common(escaped_name, m, ss);
    ss << escape_double(m.counter().value()) << std::endl;
  }

  void gauge_proto_to_ostream(std::string const& escaped_name,
			      Metric const& m, std::ostream& ss) {
    if (!m.has_gauge() || !m.gauge().has_value()) {
      throw impl::OutputFormatterException(
	impl::OutputFormatterException::kMissingRequiredField);
    }
    metric_proto_to_ostream_common(escaped_name, m, ss);
    ss << escape_double(m.gauge().value()) << std::endl;
  }

  void summary_proto_to_ostream(std::string const& escaped_name,
				Metric const& m, std::ostream& ss) {
    throw impl::OutputFormatterException(
      impl::OutputFormatterException::kSummariesNotImplemented);
  }

  void histogram_proto_to_ostream(std::string const& escaped_name,
				  Metric const& m, std::ostream& ss) {
    if (!m.has_histogram()) {
      throw impl::OutputFormatterException(
	impl::OutputFormatterException::kMissingRequiredField);
    }
    Histogram const& h = m.histogram();
    if (h.bucket_size() <= 0) {
      throw impl::OutputFormatterException(
	impl::OutputFormatterException::kMissingRequiredField);
    }
    for (int i = 0; i < h.bucket_size(); ++i) {
      Bucket const& b = h.bucket(i);
      if (!b.has_upper_bound() || !b.has_cumulative_count()) {
	throw impl::OutputFormatterException(
	  impl::OutputFormatterException::kMissingRequiredField);
      }
      ss << escaped_name << '{';
      metric_labels_proto_to_ostream(m, ss);
      if (m.label_size() > 0) {
	ss << ',';
      }
      // TODO(korfuri): Do we need quotes around the value here?
      // le="0.1" or le=0.1?
      ss << "le=\"" << escape_double(b.upper_bound())
	 << "\"} " << b.cumulative_count() << std::endl;
    }
  }

  void untyped_proto_to_ostream(std::string const& escaped_name,
				Metric const& m, std::ostream& ss) {
    if (!m.has_untyped() || !m.untyped().has_value()) {
      throw impl::OutputFormatterException(
	impl::OutputFormatterException::kMissingRequiredField);
    }
    metric_proto_to_ostream_common(escaped_name, m, ss);
    ss << escape_double(m.untyped().value()) << std::endl;
  }

  void metric_proto_to_ostream(std::string const& escaped_name,
			       Metric const& m, MetricFamily const& mf,
			       std::ostream& ss) {
    if (!mf.has_type()) {
      throw impl::OutputFormatterException(
	impl::OutputFormatterException::kMissingRequiredField);
    }
    switch (mf.type()) {
    case MetricType::COUNTER:
      counter_proto_to_ostream(escaped_name, m, ss);
      return;
    case MetricType::GAUGE:
      gauge_proto_to_ostream(escaped_name, m, ss);
      return;
    case MetricType::SUMMARY:
      summary_proto_to_ostream(escaped_name, m, ss);
      return;
    case MetricType::HISTOGRAM:
      histogram_proto_to_ostream(escaped_name, m, ss);
      return;
    case MetricType::UNTYPED:
      untyped_proto_to_ostream(escaped_name, m, ss);
      return;
    default:
      throw impl::OutputFormatterException(
	impl::OutputFormatterException::kInvalidMetricType);
    }
  }

  std::string metricfamily_proto_to_string(MetricFamily const* mf) {
    std::ostringstream ss;
    metricfamily_proto_to_ostream(ss, mf);
    return ss.str();
  }

  void metricfamily_proto_to_ostream(std::ostream& os, MetricFamily const* mf) {
    if (!mf->has_name() || !mf->has_type()) {
      throw impl::OutputFormatterException(
	impl::OutputFormatterException::kMissingRequiredField);
    }
    std::string escaped_name = escape_metric_name(mf->name());
    os << "# HELP " << escaped_name << ' ' << escape_help(mf->help())
       << std::endl;
    if (mf->has_help()) {
      os << "# TYPE " << escaped_name << ' ' << escape_type(mf->type())
	 << std::endl;
    }
    for (int i = 0; i < mf->metric_size(); ++i) {
      Metric const& m = mf->metric(i);
      metric_proto_to_ostream(escaped_name, m, *mf, os);
    }
  }

  std::string escape_metric_name(std::string const& s) {
    // Assume metric names don't need escaping as they are
    // restricted to only a few characters.
    return s;
  }

  std::string escape_help(std::string const& s) {
    icu::UnicodeString us = icu::UnicodeString::fromUTF8(s);
    us.findAndReplace("\\", "\\\\").findAndReplace("\n", "\\n");
    std::string replaced_s;
    us.toUTF8String(replaced_s);
    return replaced_s;
  }

  std::string escape_label_name(std::string const& s) {
    // Assume label names don't need escaping as they are
    // restricted to only a few characters.
    return s;
  }

  std::string escape_label_value(std::string const& s) {
    icu::UnicodeString us = icu::UnicodeString::fromUTF8(s);
    us.findAndReplace("\\", "\\\\")
      .findAndReplace("\"", "\\\"")
      .findAndReplace("\n", "\\n");
    std::string replaced_s;
    us.toUTF8String(replaced_s);
    return std::string("\"") + replaced_s + "\"";
  }

  std::string escape_double(double d) {
    if (std::isinf(d)) {
      if (d < 0) return "-Inf";
      return "+Inf";
    }
    char buf[256];
    std::snprintf(buf, 256, "%g", d);
    return std::string(buf);
  }

  namespace impl {

    const char* const OutputFormatterException::kInvalidMetricType =
      "Invalid metric type.";
    const char* const OutputFormatterException::kMissingRequiredField =
      "Missing required field.";
    const char* const OutputFormatterException::kSummariesNotImplemented =
      "Summaries are not implemented.";

  } /* namespace impl */
} /* namespace prometheus */
