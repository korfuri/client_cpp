#include "output_formatter.hh"

#include <cmath>
#include <iostream>

namespace prometheus {
  namespace impl {

    OutputFormatter::OutputFormatter(std::ostream &os) : os_(os) {}

    void OutputFormatter::addMetric(std::string const &name,
                                    std::string const &help,
                                    std::string const &type) {
      os_ << "# HELP " << escape_metric_name(name) << ' ' << escape_help(help) << std::endl;
      os_ << "# TYPE " << escape_metric_name(name) << ' ' << type << std::endl;
    }

    void OutputFormatter::addMetricValue(double value, std::string const &name) {
      os_ << escape_metric_name(name) << " = " << escape_double(value) << std::endl;
    }

    void OutputFormatter::addMetricValue(double value, double bucket, std::string const &name) {
      os_ << escape_metric_name(name) << "{le=" << escape_double(bucket) << "} = " << escape_double(value) << std::endl;
    }

    std::string OutputFormatter::escape_metric_name(std::string const& s) {
      // TODO(korfuri): Escape this properly.
      return s;
    }

    std::string OutputFormatter::escape_help(std::string const& s) {
      // TODO(korfuri): Escape this properly.
      return s;
    }

    std::string OutputFormatter::escape_label_name(std::string const& s) {
      // TODO(korfuri): Escape this properly.
      return s;
    }

    std::string OutputFormatter::escape_label_value(std::string const& s) {
      // TODO(korfuri): Escape this properly.
      return s;
    }

    std::string OutputFormatter::escape_double(double d) {
      if (std::isinf(d)) {
	if (d < 0) return "-Inf";
	return "+Inf";
      }
      char buf[256];
      std::snprintf(buf, 256, "%g", d);
      return std::string(buf);
    }

  } /* namespace impl */
} /* namespace prometheus */
