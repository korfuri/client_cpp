#include "client.hh"
#include "output_formatter.hh"
#include "utils.hh"
#include <gtest/gtest.h>
#include <fstream>
#include <functional>
#include <thread>
#include <list>

namespace {

  using namespace prometheus;

  class BenchmarkTest : public ::testing::Test {};

  SetGauge<2> test_runtime("test_runtime_last_s",
			   ("Runtime of the last run of tests, by test name"
			    " and thread count."),
			   {"test", "threads"});

  void run(std::function<void()> f, std::string const& name, int threads) {
    std::list<std::thread> l;
    prometheus::IntervalReporter<> reporter(test_runtime.labels({name, std::to_string(threads)}));
    for (int i = 0; i < threads; ++i) {
      l.push_back(std::thread(f));
    }
    for (auto& t : l) {
      t.join();
    }
  }


  Counter<0> counter0("counter0", "");
  void incCounter1000000() {
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

  IncDecGauge<0> gauge0("gauge0", "");
  void incGauge1000000() {
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

}

SetGauge<0> run_time("run_timestamp", "Timestamp at which this test was run.");

int main(int argc, char**argv) {
  ::testing::InitGoogleTest(&argc, argv);
  prometheus::set_to_current_time(run_time);
  int retval = RUN_ALL_TESTS();
  {
    std::fstream bin_output(std::string(getenv("TEST_TMPDIR")) + "/benchmark_report.pb", std::ios::out | std::ios::binary);
    auto v = prometheus::impl::global_registry.collect();
    for (auto m : v) {
      prometheus::metricfamily_proto_to_ostream(std::cout, m);
      prometheus::delete_metricfamily(m);
    }
  }
  return retval;
};
