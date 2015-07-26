#include "client.hh"
#include <string>
#include <gtest/gtest.h>

namespace {

  using namespace prometheus;

  class ClientCPPTest : public ::testing::Test {};

  Counter<0> c0("test_counter0", "test Counter<0>");
  Counter<1> c1("test_counter1", "test Counter<1>", {{"x"}});
  Counter<2> c2("test_counter2", "test Counter<2>", {"x", "y"});

  TEST_F(ClientCPPTest, CounterTest) {
    EXPECT_EQ(0, c0.value());
    EXPECT_EQ(0, c1.labels({"a"}).value());
    EXPECT_EQ(0, c2.labels({"a", "a"}).value());

    c0.inc();
    c1.labels({"b"}).inc(2.3);
    c2.labels({"c", "c"}).inc();
    EXPECT_EQ(1.0, c0.value());
    EXPECT_EQ(0, c1.labels({"a"}).value());
    EXPECT_EQ(2.3, c1.labels({"b"}).value());
    EXPECT_EQ(0, c2.labels({"a", "a"}).value());
    EXPECT_EQ(0, c2.labels({"a", "c"}).value());
    EXPECT_EQ(0, c2.labels({"c", "a"}).value());
    EXPECT_EQ(1.0, c2.labels({"c", "c"}).value());

    // Decrementing a counter throws.
    EXPECT_THROW(c0.inc(-1), err::NegativeCounterIncrementException);
    EXPECT_NO_THROW(c0.inc(0));
  }

  Gauge<0> g0("test_gauge0", "test Gauge<0>");
  Gauge<1> g1("test_gauge1", "test Gauge<1>", {{"x"}});
  Gauge<2> g2("test_gauge2", "test Gauge<2>", {"x", "y"});

  TEST_F(ClientCPPTest, GaugeTest) {
    EXPECT_EQ(0, g0.value());
    EXPECT_EQ(0, g1.labels({"a"}).value());
    EXPECT_EQ(0, g2.labels({"a", "a"}).value());

    g0.set(4.2);
    g1.labels({"b"}).set(1.1);
    g2.labels({"c", "c"}).set(2.4);
    EXPECT_EQ(4.2, g0.value());
    EXPECT_EQ(0, g1.labels({"a"}).value());
    EXPECT_EQ(1.1, g1.labels({"b"}).value());
    EXPECT_EQ(0, g2.labels({"a", "a"}).value());
    EXPECT_EQ(0, g2.labels({"a", "c"}).value());
    EXPECT_EQ(0, g2.labels({"c", "a"}).value());
    EXPECT_EQ(2.4, g2.labels({"c", "c"}).value());

    g0.set(-1.2);
    EXPECT_EQ(-1.2, g0.value());
  }

  Histogram<0> h0("test_histogram0", "test Histogram<0>");
  Histogram<0> h0a("test_histogram0a", "test Histogram<0> with custom levels",
                   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, kInf});
  Histogram<0> h0b("test_histogram0b",
                   "test Histogram<0> with custom levels without infinity",
                   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
  Histogram<1> h1("test_histogram1", "test Histogram<1>", {{"x"}});

  TEST_F(ClientCPPTest, LevelUpTest) {
    EXPECT_EQ("1.000000", h0a.level_up(0.9));
    EXPECT_EQ("1.000000", h0a.level_up(1));
    EXPECT_EQ("2.000000", h0a.level_up(1.1));
    EXPECT_EQ("+Inf", h0a.level_up(11));
  }

  TEST_F(ClientCPPTest, UnlabelledHistogramTest) {
    h0.record(1.5);
    h0.record(7.5);
    h0a.record(1.5);
    h0a.record(7.5);
    h0b.record(1.5);
    h0b.record(7.5);

    EXPECT_EQ(0, h0.labels({"0.050000"}).value());
    EXPECT_EQ(0, h0a.labels({"0.050000"}).value());
    EXPECT_EQ(0, h0b.labels({"0.050000"}).value());

    EXPECT_EQ(1, h0.labels({"5.000000"}).value());
    EXPECT_EQ(1, h0a.labels({"5.000000"}).value());
    EXPECT_EQ(1, h0b.labels({"5.000000"}).value());

    EXPECT_EQ(0, h0.labels({"6.000000"}).value());
    EXPECT_EQ(2, h0.labels({"7.500000"}).value());
    EXPECT_EQ(1, h0a.labels({"6.000000"}).value());
    EXPECT_EQ(1, h0b.labels({"6.000000"}).value());

    EXPECT_EQ(2, h0.labels({h0.level_up(7.5)}).value());
    EXPECT_EQ(2, h0a.labels({h0a.level_up(7.5)}).value());
    EXPECT_EQ(2, h0b.labels({h0b.level_up(7.5)}).value());

    EXPECT_EQ(2, h0.labels({kInfStr}).value());
    EXPECT_EQ(2, h0a.labels({kInfStr}).value());
    EXPECT_EQ(2, h0b.labels({kInfStr}).value());
  }

  TEST_F(ClientCPPTest, LabelledHistogramTest) {
    h1.record(4.2, {"a"});
    h1.record(4.6, {"b"});
    h1.record(5.6, {"b"});

    EXPECT_EQ(1, h1.labels({"a", h1.level_up(4.2)}).value());
    EXPECT_EQ(1, h1.labels({"b", h1.level_up(4.6)}).value());
    EXPECT_EQ(2, h1.labels({"b", h1.level_up(5.6)}).value());

    EXPECT_EQ(1, h1.labels({"a", kInfStr}).value());
    EXPECT_EQ(2, h1.labels({"b", kInfStr}).value());
  }

  TEST_F(ClientCPPTest, BadHistogramLevelsTest) {
    EXPECT_THROW(new Histogram<0>("test", "test", {3,2,1,0}), err::UnsortedLevelsException);
  }
} /* namespace */

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
