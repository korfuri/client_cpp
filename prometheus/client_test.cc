#include "client.hh"
#include <string>
#include <gtest/gtest.h>

namespace {

using namespace prometheus;

class ClientCPPTest : public ::testing::Test {};

Counter<0> c0("test_counter0");
Counter<1> c1("test_counter1", {{"x"}});
Counter<2> c2("test_counter2", {"x", "y"});

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

  // TODO(korfuri): Test that inc(x) for x<0 throws an exception.
}

Gauge<0> g0("test_gauge0");
Gauge<1> g1("test_gauge1", {{"x"}});
Gauge<2> g2("test_gauge2", {"x", "y"});

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

// TODO(korfuri): Test histograms.

} /* namespace */

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
