#include "client.hh"
#include <gtest/gtest.h>
#include <list>
#include <string>
#include <thread>

namespace {

  using namespace prometheus;

  class ClientConcurrentTest : public ::testing::Test {};

  Counter<0> c0("test_counter0", "Test Counter<0>.");
  Counter<1> c1("test_counter1", "Test Counter<1>.", {{"even"}});

  const int kIterations = 1000;
  const int kThreads = 40;

  void f_countertest(int threadid) {
    for (int i = 0; i < kIterations; ++i) {
      c0.inc();
      c1.labels({{std::to_string(bool(threadid % 2))}}).inc();
    }
  }

  TEST_F(ClientConcurrentTest, CounterTest) {
    std::list<std::thread> l;
    for (int i = 0; i < kThreads; ++i) {
      l.push_back(std::thread(f_countertest, i));
    }
    for (auto &t : l) {
      t.join();
    }
    EXPECT_EQ(kThreads * kIterations, c0.value());
    EXPECT_EQ(kThreads * kIterations / 2, c1.labels({{"0"}}).value());
    EXPECT_EQ(kThreads * kIterations / 2, c1.labels({{"1"}}).value());
  }

  Gauge<0> g0("test_gauge0", "Test Gauge<0>.");
  Gauge<1> g1("test_gauge1", "Test Gauge<1>.", {{"even"}});

  void f_gaugetest(int threadid) {
    g0.set(threadid);
    g1.labels({{std::to_string(bool(threadid % 2))}}).set(threadid);
  }

  TEST_F(ClientConcurrentTest, GaugeTest) {
    std::list<std::thread> l;
    for (int i = 0; i < kThreads; ++i) {
      l.push_back(std::thread(f_gaugetest, i));
    }
    for (auto &t : l) {
      t.join();
    }
    EXPECT_TRUE(g0.value() >= 0 && g0.value() < kThreads);
    double g1_even = g1.labels({{"0"}}).value();
    EXPECT_TRUE(g1_even >= 0);
    EXPECT_TRUE(g1_even < kThreads);
    EXPECT_EQ(0, (int)g1_even % 2);
    double g1_odd = g1.labels({{"1"}}).value();
    EXPECT_TRUE(g1_odd >= 0);
    EXPECT_TRUE(g1_odd < kThreads);
    EXPECT_EQ(1, (int)g1_odd % 2);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
