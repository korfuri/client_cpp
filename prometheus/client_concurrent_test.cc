#include "client.hh"
#include "utils.hh"
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
    for (auto& t : l) {
      t.join();
    }
    EXPECT_EQ(kThreads * kIterations, c0.value());
    EXPECT_EQ(kThreads * kIterations / 2, c1.labels({{"0"}}).value());
    EXPECT_EQ(kThreads * kIterations / 2, c1.labels({{"1"}}).value());
  }

  SetGauge<0> sg0("test_set_gauge0", "Test SetGauge<0>.");
  SetGauge<1> sg1("test_set_gauge1", "Test SetGauge<1>.", {{"even"}});

  void f_setgaugetest(int threadid) {
    sg0.set(threadid);
    sg1.labels({{std::to_string(bool(threadid % 2))}}).set(threadid);
  }

  TEST_F(ClientConcurrentTest, SetGaugeTest) {
    std::list<std::thread> l;
    for (int i = 0; i < kThreads; ++i) {
      l.push_back(std::thread(f_setgaugetest, i));
    }
    for (auto& t : l) {
      t.join();
    }
    EXPECT_TRUE(sg0.value() >= 0 && sg0.value() < kThreads);
    double g1_even = sg1.labels({{"0"}}).value();
    EXPECT_TRUE(g1_even >= 0);
    EXPECT_TRUE(g1_even < kThreads);
    EXPECT_EQ(0, (int)g1_even % 2);
    double g1_odd = sg1.labels({{"1"}}).value();
    EXPECT_TRUE(g1_odd >= 0);
    EXPECT_TRUE(g1_odd < kThreads);
    EXPECT_EQ(1, (int)g1_odd % 2);
  }

  IncDecGauge<0> idg0("test_incdec_gauge0", "Test IncDecGauge<0>.");
  IncDecGauge<0> idg_inprogress("test_inprogress", "Test InProgress.");
  IncDecGauge<0> idg0a("test_incdec_gauge0a", "Test IncDecGauge<0>.");
  IncDecGauge<1> idg1("test_incdec_gauge1", "Test IncDecGauge<1>.",
                      {"threadid"});

  void f_incdecgaugetest(int threadid) {
    std::string id = std::to_string(threadid);
    for (int i = 0; i < kIterations; ++i) {
      InProgress ip(idg_inprogress);
      idg0.inc(threadid);
      idg0a.inc();
      idg1.labels({id}).inc();
      idg0.dec(threadid);
      idg1.labels({id}).dec();
    }
  }

  TEST_F(ClientConcurrentTest, IncDecGaugeTest) {
    std::list<std::thread> l;
    for (int i = 0; i < kThreads; ++i) {
      EXPECT_EQ(0, idg1.labels({std::to_string(i)}).value());
      l.push_back(std::thread(f_incdecgaugetest, i));
    }
    for (auto& t : l) {
      t.join();
    }
    EXPECT_EQ(0, idg0.value());
    EXPECT_EQ(0, idg_inprogress.value());
    EXPECT_EQ(kThreads * kIterations, idg0a.value());
    for (int i = 0; i < kThreads; ++i) {
      EXPECT_EQ(0, idg1.labels({std::to_string(i)}).value());
    }
  }

  Histogram<1> h1("test_histogram1", "Test Histogram<1>",
		  {"threadgroup"});
  Histogram<1> h1a("test_histogram1a", "Test Histogram<1>",
		   {"x"}, histogram_levels({5, 10, 15, 20, 25, 30, 40, 50, 60, 100}));

  void f_histogramtest(int threadid) {
    std::string threadgroup = std::to_string(threadid % 4);
    for (int i = 0; i < kIterations; ++i) {
      h1.labels({threadgroup}).observe((double)i / 1000.0);
      h1a.labels({std::to_string(i)}).observe(threadid);
    }
  }

  TEST_F(ClientConcurrentTest, HistogramTest) {
    std::list<std::thread> l;
    for (int i = 0; i < kThreads; ++i) {
      l.push_back(std::thread(f_histogramtest, i));
    }
    for (auto& t : l) {
      t.join();
    }
    EXPECT_EQ(kIterations * kThreads / 4, h1.labels({"0"}).value());
    EXPECT_EQ(kIterations * kThreads / 4, h1.labels({"1"}).value());
    EXPECT_EQ(kIterations * kThreads / 4, h1.labels({"2"}).value());
    EXPECT_EQ(kIterations * kThreads / 4, h1.labels({"3"}).value());
    EXPECT_EQ(0, h1.labels({"4"}).value());
    for (int i = 0; i < kIterations; ++i) {
      EXPECT_EQ(11, h1a.labels({std::to_string(i)}).value(10));
      EXPECT_EQ(kThreads, h1a.labels({std::to_string(i)}).value());
    }
  }
}
