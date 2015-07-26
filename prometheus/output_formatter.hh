#ifndef PROMETHEUS_OUTPUT_FORMATTER_HH__
#define PROMETHEUS_OUTPUT_FORMATTER_HH__

#include <ostream>

namespace prometheus {

namespace impl {

class OutputFormatter {
 public:
  OutputFormatter(std::ostream& os);

  void addMetric(std::string const& name, std::string const& help,
                 std::string const& type);

  void addMetricValue(std::string const& name, double value);

  template <typename LabelIterator>
  void addMetricLabelRow(std::string const& name,
                         LabelIterator const& labels_begin,
                         LabelIterator const& labels_end, double value) {
    if (labels_begin == labels_end) {
      os_ << name << " = " << value << std::endl;
    } else {
      os_ << name;
      char nextchar = '{';
      for (auto it = labels_begin; it != labels_end; ++it) {
        os_ << nextchar << (*it).first << '=' << (*it).second;
        nextchar = ',';
      }
      os_ << "} = " << value << std::endl;
    }
  }

 private:
  std::ostream& os_;
};
}
}

#endif
