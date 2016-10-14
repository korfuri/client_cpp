#include "client.hh"
#include "output_formatter.hh"
#include "utils.hh"
#include <gtest/gtest.h>
#include <fstream>
#include <functional>
#include <thread>
#include <list>
#include <random>

namespace {

  using namespace prometheus;
  using namespace std::placeholders;

  class BenchmarkTest : public ::testing::Test {};

  SetGauge<2> test_runtime("test_runtime_last_s",
                           "Runtime of the last run of tests, by test name"
                           " and thread count.",
                           {"test", "threads"});

  // This is the harness that runs the actual benchmarks. It spawns
  // `threads` threads and makes them run the benchmark function, then
  // joins them all. It uses an IntervalReporter to measure the total
  // time spent.
  void run(std::function<void(int, int)> f, std::string const& name, int threads) {
    std::list<std::thread> l;
    prometheus::IntervalReporter<> reporter(test_runtime.labels({name, std::to_string(threads)}));
    for (int i = 0; i < threads; ++i) {
      l.push_back(std::thread(f, i, threads));
    }
    for (auto& t : l) {
      t.join();
    }
  }


  // Test incrementing a counter many times.
  Counter<0> counter0("counter0", "");
  void incCounter1000000(int threadid, int threadcount) {
    for (int i = 0; i < 1000000; ++i) {
      counter0.inc();
    }
  }
  TEST_F(BenchmarkTest, IncCounter) {
    run(incCounter1000000, "incCounter1000000", 1);
    EXPECT_EQ(1000000, counter0.value());
    run(incCounter1000000, "incCounter1000000", 10);
    EXPECT_EQ(11000000, counter0.value());
    run(incCounter1000000, "incCounter1000000", 100);
    EXPECT_EQ(111000000, counter0.value());
  }

  // Test incrementing a gauge many times.
  IncDecGauge<0> gauge0("gauge0", "");
  void incGauge1000000(int threadid, int threadcount) {
    for (int i = 0; i < 1000000; ++i) {
      gauge0.inc();
    }
  }
  TEST_F(BenchmarkTest, IncGauge) {
    run(incGauge1000000, "incGauge1000000", 1);
    EXPECT_EQ(1000000, gauge0.value());
    run(incGauge1000000, "incGauge1000000", 10);
    EXPECT_EQ(11000000, gauge0.value());
    run(incGauge1000000, "incGauge1000000", 100);
    EXPECT_EQ(111000000, gauge0.value());
  }

  // Test setting a gauge many times.
  SetGauge<0> gauge1("gauge1", "");
  void setGauge1000000(int threadid, int threadcount) {
    for (int i = 0; i < 1000000; ++i) {
      gauge1.set(i);
    }
  }
  TEST_F(BenchmarkTest, SetGauge) {
    run(setGauge1000000, "setGauge1000000", 1);
    EXPECT_EQ(1000000-1, gauge1.value());
    run(setGauge1000000, "setGauge1000000", 10);
    EXPECT_EQ(1000000-1, gauge1.value());
    run(setGauge1000000, "setGauge1000000", 100);
    EXPECT_EQ(1000000-1, gauge1.value());
  }

  // Test creating new valuetypes (counters, here) and performing an
  // operation on them.
  Counter<3> labelled_counter0("labelled_counter0", "", {
      "threadid", "threadcount", "iteration"});
  void createLabelsAndInc10000(int threadid, int threadcount) {
    std::string threadid_str = std::to_string(threadid);
    std::string threadcount_str = std::to_string(threadcount);
    for (int i = 0; i < 10000; ++i) {
      labelled_counter0.labels({threadid_str, threadcount_str, std::to_string(i)}).inc();
    }
  }
  TEST_F(BenchmarkTest, CreateLabelsAndInc) {
    run(createLabelsAndInc10000, "createLabelsAndInc10000", 1);
    EXPECT_EQ(1, labelled_counter0.labels({"0", "1", "50"}).value());

    run(createLabelsAndInc10000, "createLabelsAndInc10000", 10);
    EXPECT_EQ(1, labelled_counter0.labels({"0", "10", "960"}).value());
    EXPECT_EQ(1, labelled_counter0.labels({"5", "10", "5324"}).value());

    run(createLabelsAndInc10000, "createLabelsAndInc10000", 100);
    EXPECT_EQ(1, labelled_counter0.labels({"0", "100", "0"}).value());
    EXPECT_EQ(1, labelled_counter0.labels({"99", "100", "9999"}).value());
  }

  // Test observing pseudorandom values (with a fixed seed for
  // reproducibility). The distribution is passed as an argument, to
  // compare uniform distribution (cheaper) vs exponential
  // distribution (more realistic).
  Histogram<0> histogram0("histogram0", "");
  template<typename distribution>
  void observePseudoRandom100000(distribution& dis, int threadid, int threadcount) {
    std::mt19937 prng(42);
    for (int i = 0; i < 100000; ++i) {
      double d = dis(prng);
      histogram0.observe(d);
    }
  }
  TEST_F(BenchmarkTest, Histogram) {
    std::uniform_real_distribution<double> uniform(0.0, 1024.0);
    run(std::bind(observePseudoRandom100000<std::uniform_real_distribution<double>>, uniform, _1, _2),
        "observepseudorandom100000<uniform>", 1);
    EXPECT_EQ(100000, histogram0.value());
    run(std::bind(observePseudoRandom100000<std::uniform_real_distribution<double>>, uniform, _1, _2),
        "observepseudorandom100000<uniform>", 10);
    EXPECT_EQ(11*100000, histogram0.value());
    run(std::bind(observePseudoRandom100000<std::uniform_real_distribution<double>>, uniform, _1, _2),
        "observepseudorandom100000<uniform>", 100);
    EXPECT_EQ(111*100000, histogram0.value());

    std::exponential_distribution<double> exponential(1.0);
    run(std::bind(observePseudoRandom100000<std::exponential_distribution<double>>, exponential, _1, _2),
        "observepseudorandom100000<exponential>", 1);
    EXPECT_EQ(112*100000, histogram0.value());
    run(std::bind(observePseudoRandom100000<std::exponential_distribution<double>>, exponential, _1, _2),
        "observepseudorandom100000<exponential>", 10);
    EXPECT_EQ(122*100000, histogram0.value());
    run(std::bind(observePseudoRandom100000<std::exponential_distribution<double>>, exponential, _1, _2),
        "observepseudorandom100000<exponential>", 100);
    EXPECT_EQ(222*100000, histogram0.value());
  }

}

SetGauge<0> run_time("run_timestamp", "Timestamp at which this test was run.");

int main(int argc, char**argv) {
  ::testing::InitGoogleTest(&argc, argv);
  prometheus::set_to_current_time(run_time);
  int retval = RUN_ALL_TESTS();
  {
    auto v = prometheus::impl::global_registry.collect();
    for (auto m : v) {
      if (m->name() == "test_runtime_last_s") {
        prometheus::metricfamily_proto_to_ostream(std::cout, m);
      }
    }
  }
  return retval;
};
