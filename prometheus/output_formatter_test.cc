#include "gtest/gtest.h"
#include "google/protobuf/text_format.h"
#include "output_formatter.hh"

#include <string>

namespace {
  using namespace prometheus;
  using namespace prometheus::impl;

  class OutputFormatterTest : public ::testing::Test {};

  TEST_F(OutputFormatterTest, CounterTest) {
    MetricFamily mf;
    EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
        "name: \"a\" help: \"b\" type: COUNTER metric: { counter: { value: 4.2 "
        "} }",
        &mf));
    std::string s;
    EXPECT_NO_THROW(s = metricfamily_proto_to_string(&mf));
    EXPECT_EQ(
        "# HELP a b\n"
        "# TYPE a counter\n"
        "a 4.2\n",
        s);
  }

  TEST_F(OutputFormatterTest, GaugeTest) {
    MetricFamily mf;
    EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
        "name: \"a\" help: \"b\" type: GAUGE metric: { gauge: { value: 4.2 } }",
        &mf));
    std::string s;
    EXPECT_NO_THROW(s = metricfamily_proto_to_string(&mf));
    EXPECT_EQ(
        "# HELP a b\n"
        "# TYPE a gauge\n"
        "a 4.2\n",
        s);
  }

  TEST_F(OutputFormatterTest, UntypedTest) {
    MetricFamily mf;
    EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
        "name: \"a\" help: \"b\" type: UNTYPED metric: { untyped: { value: 4.2 "
        "} }",
        &mf));
    std::string s;
    EXPECT_NO_THROW(s = metricfamily_proto_to_string(&mf));
    EXPECT_EQ(
        "# HELP a b\n"
        "# TYPE a untyped\n"
        "a 4.2\n",
        s);
  }

  TEST_F(OutputFormatterTest, HistogramTest) {
    MetricFamily mf;
    EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
        "name: \"a\" help: \"b\" type: HISTOGRAM metric: { histogram: { bucket "
        "{ upper_bound: 4.2 cumulative_count: 2 } } }",
        &mf));
    std::string s;
    EXPECT_NO_THROW(s = metricfamily_proto_to_string(&mf));
  }

  TEST_F(OutputFormatterTest, EmptyMetricFamily) {
    MetricFamily mf;
    EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString("", &mf));
    EXPECT_THROW(metricfamily_proto_to_string(&mf), OutputFormatterException);
  }

  TEST_F(OutputFormatterTest, NoMetric) {
    MetricFamily mf;
    EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
        "name: \"a\" help: \"b\" type: COUNTER", &mf));
    EXPECT_NO_THROW(metricfamily_proto_to_string(&mf));
  }

  TEST_F(OutputFormatterTest, NoType) {
    MetricFamily mf;
    EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
        "name: \"a\" help: \"b\" metric: {}", &mf));
    EXPECT_THROW(metricfamily_proto_to_string(&mf), OutputFormatterException);
  }

  TEST_F(OutputFormatterTest, CounterTypeNoCounterMetric) {
    MetricFamily mf;
    EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
        "name: \"a\" help: \"b\" type: COUNTER metric: {}", &mf));
    EXPECT_THROW(metricfamily_proto_to_string(&mf), OutputFormatterException);
  }

  TEST_F(OutputFormatterTest, GaugeTypeNoGaugeMetric) {
    MetricFamily mf;
    EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
        "name: \"a\" help: \"b\" type: GAUGE metric: {}", &mf));
    EXPECT_THROW(metricfamily_proto_to_string(&mf), OutputFormatterException);
  }

  TEST_F(OutputFormatterTest, SummaryTypeNoSummaryMetric) {
    MetricFamily mf;
    EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
        "name: \"a\" help: \"b\" type: SUMMARY metric: {}", &mf));
    EXPECT_THROW(metricfamily_proto_to_string(&mf), OutputFormatterException);
  }

  TEST_F(OutputFormatterTest, UntypedTypeNoUntypedMetric) {
    MetricFamily mf;
    EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
        "name: \"a\" help: \"b\" type: UNTYPED metric: {}", &mf));
    EXPECT_THROW(metricfamily_proto_to_string(&mf), OutputFormatterException);
  }

  TEST_F(OutputFormatterTest, HistogramTypeNoHistogramMetric) {
    MetricFamily mf;
    EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
        "name: \"a\" help: \"b\" type: HISTOGRAM metric: {}", &mf));
    EXPECT_THROW(metricfamily_proto_to_string(&mf), OutputFormatterException);
  }

  TEST_F(OutputFormatterTest, CounterNoValue) {
    MetricFamily mf;
    EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
        "name: \"a\" help: \"b\" type: COUNTER metric: { counter: {} }", &mf));
    EXPECT_THROW(metricfamily_proto_to_string(&mf), OutputFormatterException);
  }

  TEST_F(OutputFormatterTest, GaugeNoValue) {
    MetricFamily mf;
    EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
        "name: \"a\" help: \"b\" type: GAUGE metric: { gauge: {} }", &mf));
    EXPECT_THROW(metricfamily_proto_to_string(&mf), OutputFormatterException);
  }

  TEST_F(OutputFormatterTest, UntypedNoValue) {
    MetricFamily mf;
    EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
        "name: \"a\" help: \"b\" type: UNTYPED metric: { untyped: {} }", &mf));
    EXPECT_THROW(metricfamily_proto_to_string(&mf), OutputFormatterException);
  }

  TEST_F(OutputFormatterTest, HistogramNoBuckets) {
    MetricFamily mf;
    EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
        "name: \"a\" help: \"b\" type: HISTOGRAM metric: { histogram: {} }",
        &mf));
    EXPECT_THROW(metricfamily_proto_to_string(&mf), OutputFormatterException);
  }

  TEST_F(OutputFormatterTest, SummaryNoQuantiles) {
    MetricFamily mf;
    EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
        "name: \"a\" help: \"b\" type: SUMMARY metric: { summary: {} }", &mf));
    EXPECT_THROW(metricfamily_proto_to_string(&mf), OutputFormatterException);
  }

  TEST_F(OutputFormatterTest, HistogramBucketNoCount) {
    MetricFamily mf;
    EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
        "name: \"a\" help: \"b\" type: HISTOGRAM metric: { histogram: { bucket "
        "{ upper_bound: 4.2 } } }",
        &mf));
    EXPECT_THROW(metricfamily_proto_to_string(&mf), OutputFormatterException);
  }

  TEST_F(OutputFormatterTest, HistogramBucketNoUpperBound) {
    MetricFamily mf;
    EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
        "name: \"a\" help: \"b\" type: HISTOGRAM metric: { histogram: { bucket "
        "{ cumulative_count: 2 } } }",
        &mf));
    EXPECT_THROW(metricfamily_proto_to_string(&mf), OutputFormatterException);
  }

  TEST_F(OutputFormatterTest, LabelValueEscaping) {
    EXPECT_EQ("\"abcd\"", escape_label_value("abcd"));
    EXPECT_EQ("\"a\\\\b\\\"c\\nd\"", escape_label_value("a\\b\"c\nd"));
  }

  TEST_F(OutputFormatterTest, LabelValueUnicodeEscaping) {
    // "  is u+0022, 丢 is u+4e22
    // \  is u+005c, 乜 is u+4e5c
    // \n is u+000a, 上 is u+4e0a
    EXPECT_EQ(u8"\"丢乜上x\"", escape_label_value(u8"丢乜上x"));
    EXPECT_EQ(u8"\"丢\\\\乜\\\"上\\nx\"",
              escape_label_value(u8"丢\\乜\"上\nx"));
  }

  TEST_F(OutputFormatterTest, HelpTextEscaping) {
    EXPECT_EQ("abcd", escape_help("abcd"));
    EXPECT_EQ("a\\\\b\"c\\nd", escape_help("a\\b\"c\nd"));
  }

  TEST_F(OutputFormatterTest, HelpTextUnicodeEscaping) {
    // "  is u+0022, 丢 is u+4e22
    // \  is u+005c, 乜 is u+4e5c
    // \n is u+000a, 上 is u+4e0a
    EXPECT_EQ(u8"丢乜上x", escape_help(u8"丢乜上x"));
    EXPECT_EQ(u8"丢\\\\乜\"上\\nx", escape_help(u8"丢\\乜\"上\nx"));
  }
}
