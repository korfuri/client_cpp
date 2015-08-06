/* -*- mode: C++; coding: utf-8-unix -*- */
#include "client.hh"
#include "utils.hh"
#include "external/fake_clock/fake_clock.hh"
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

  SetGauge<0> sg0("test_set_gauge0", "test SetGauge<0>");
  SetGauge<1> sg1("test_set_gauge1", "test SetGauge<1>", {{"x"}});
  SetGauge<2> sg2("test_set_gauge2", "test SetGauge<2>", {"x", "y"});

  TEST_F(ClientCPPTest, SetGaugeTest) {
    EXPECT_EQ(0, sg0.value());
    EXPECT_EQ(0, sg1.labels({"a"}).value());
    EXPECT_EQ(0, sg2.labels({"a", "a"}).value());

    sg0.set(4.2);
    sg1.labels({"b"}).set(1.1);
    sg2.labels({"c", "c"}).set(2.4);
    EXPECT_EQ(4.2, sg0.value());
    EXPECT_EQ(0, sg1.labels({"a"}).value());
    EXPECT_EQ(1.1, sg1.labels({"b"}).value());
    EXPECT_EQ(0, sg2.labels({"a", "a"}).value());
    EXPECT_EQ(0, sg2.labels({"a", "c"}).value());
    EXPECT_EQ(0, sg2.labels({"c", "a"}).value());
    EXPECT_EQ(2.4, sg2.labels({"c", "c"}).value());

    sg0.set(-1.2);
    EXPECT_EQ(-1.2, sg0.value());
  }

  IncDecGauge<0> idg0("test_incdec_gauge0", "test IncDecGauge<0>");
  IncDecGauge<1> idg1("test_incdec_gauge1", "test IncDecGauge<1>", {{"x"}});
  IncDecGauge<2> idg2("test_incdec_gauge2", "test IncDecGauge<2>", {"x", "y"});

  TEST_F(ClientCPPTest, IncdecGaugeTest) {
    EXPECT_EQ(0, idg0.value());
    EXPECT_EQ(0, idg1.labels({"a"}).value());
    EXPECT_EQ(0, idg2.labels({"a", "a"}).value());

    idg0.inc(4.2);
    idg1.labels({"b"}).inc(1.1);
    idg2.labels({"c", "c"}).inc(2.4);
    EXPECT_EQ(4.2, idg0.value());
    EXPECT_EQ(0, idg1.labels({"a"}).value());
    EXPECT_EQ(1.1, idg1.labels({"b"}).value());
    EXPECT_EQ(0, idg2.labels({"a", "a"}).value());
    EXPECT_EQ(0, idg2.labels({"a", "c"}).value());
    EXPECT_EQ(0, idg2.labels({"c", "a"}).value());
    EXPECT_EQ(2.4, idg2.labels({"c", "c"}).value());

    idg0.inc(-1.2);
    EXPECT_EQ(3, idg0.value());
    idg0.dec(0.5);
    EXPECT_EQ(2.5, idg0.value());

    {
      InProgress _0(idg0);
      InProgress _2(idg2.labels({"c", "c"}));
      EXPECT_EQ(3.5, idg0.value());
      EXPECT_EQ(3.4, idg2.labels({"c", "c"}).value());
    }
    EXPECT_EQ(2.5, idg0.value());
    EXPECT_EQ(2.4, idg2.labels({"c", "c"}).value());
  }

  Histogram<0> h0("test_histogram0", "test Histogram<0>");
  Histogram<0> h0a("test_histogram0a", "test Histogram<0> with custom levels",
                   histogram_levels({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, kInf}));
  Histogram<0> h0b("test_histogram0b",
                   "test Histogram<0> with custom levels without infinity",
                   histogram_levels({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));
  Histogram<1> h1("test_histogram1", "test Histogram<1>", {{"x"}});

  TEST_F(ClientCPPTest, UnlabelledHistogramTest) {
    h0.record(1.5);
    h0.record(7.5);
    h0a.record(1.5);
    h0a.record(7.5);
    h0b.record(1.5);
    h0b.record(7.5);

    EXPECT_EQ(0, h0.value(0.05));
    EXPECT_EQ(0, h0a.value(0.05));
    EXPECT_EQ(0, h0b.value(0.05));

    EXPECT_EQ(1, h0.value(5));
    EXPECT_EQ(1, h0a.value(5));
    EXPECT_EQ(1, h0b.value(5));

    EXPECT_EQ(2, h0.value(6));
    EXPECT_EQ(2, h0.value(7.5));
    EXPECT_EQ(1, h0a.value(6));
    EXPECT_EQ(1, h0b.value(6));

    EXPECT_EQ(2, h0.value(7.5));
    EXPECT_EQ(2, h0a.value(7.5));
    EXPECT_EQ(2, h0b.value(7.5));

    EXPECT_EQ(2, h0.value());
    EXPECT_EQ(2, h0a.value());
    EXPECT_EQ(2, h0b.value());

    EXPECT_EQ(2, h0.value(kInf));
    EXPECT_EQ(2, h0a.value(kInf));
    EXPECT_EQ(2, h0b.value(kInf));
  }

  TEST_F(ClientCPPTest, LabelledHistogramTest) {
    h1.labels({"a"}).record(4.2);
    h1.labels({"b"}).record(4.6);
    h1.labels({"b"}).record(5.6);

    EXPECT_EQ(1, h1.labels({"a"}).value(4.2));
    EXPECT_EQ(1, h1.labels({"b"}).value(4.6));
    EXPECT_EQ(2, h1.labels({"b"}).value(5.6));

    EXPECT_EQ(1, h1.labels({"a"}).value());
    EXPECT_EQ(2, h1.labels({"b"}).value());

    EXPECT_EQ(1, h1.labels({"a"}).value(kInf));
    EXPECT_EQ(2, h1.labels({"b"}).value(kInf));
  }

  TEST_F(ClientCPPTest, BadHistogramLevelsTest) {
    EXPECT_THROW(
        new Histogram<0>("test", "test", histogram_levels({3, 2, 1, 0})),
        err::UnsortedLevelsException);
  }

  TEST_F(ClientCPPTest, HistogramLevelsTest) {
    EXPECT_EQ(std::vector<double>({0, 1, 2, 4, 8, kInf}),
              (histogram_levels_powers_of(2, 4)));
    EXPECT_EQ(std::vector<double>({0, 1, 2, 4, 8, kInf}),
              histogram_levels_powers_of(2, 4));
    EXPECT_ANY_THROW(histogram_levels_powers_of(2, 0));
    EXPECT_ANY_THROW(histogram_levels_powers_of(2, -8));
  }

  TEST_F(ClientCPPTest, BadMetricNamesTest) {
    EXPECT_THROW(new Counter<0>("", ""), err::InvalidNameException);
    EXPECT_THROW(new Counter<0>("dashed-name", ""), err::InvalidNameException);
    EXPECT_THROW(new Counter<0>("dashed-name", ""), err::InvalidNameException);
    EXPECT_NO_THROW(new Counter<0>("_underscore_prefix", ""));
    EXPECT_THROW(new Counter<0>("__reserved_prefix", ""),
                 err::InvalidNameException);
    EXPECT_THROW(new Counter<0>("bad_+_char", ""), err::InvalidNameException);
    EXPECT_THROW(new Counter<0>(u8"üñíçøđè", ""), err::InvalidNameException);
  }

  TEST_F(ClientCPPTest, BadLabelNamesTest) {
    EXPECT_THROW(new Counter<1>("a", "", {""}), err::InvalidNameException);
    EXPECT_THROW(new Counter<1>("a", "", {"dashed-name"}),
                 err::InvalidNameException);
    EXPECT_NO_THROW(new Counter<1>("a", "", {"underscore_name"}));
    EXPECT_THROW(new Counter<1>("a", "", {"bad_+_char"}),
                 err::InvalidNameException);
    EXPECT_THROW(new Counter<1>("a", "", {u8"üñíçøđè"}),
                 err::InvalidNameException);
    // Reserved names
    EXPECT_THROW(new Counter<1>("a", "", {"le"}), err::InvalidNameException);
    EXPECT_THROW(new Counter<1>("a", "", {"quantile"}),
                 err::InvalidNameException);
  }

  Histogram<0> histogram_elapsed_time("elapsed_time_ms", "",
                                      histogram_levels_powers_of(10, 10));

  TEST_F(ClientCPPTest, IntervalAccumulatorTest) {
    EXPECT_EQ(0, histogram_elapsed_time.value());
    { IntervalAccumulator<testing::fake_clock> _(histogram_elapsed_time); }
    EXPECT_EQ(1, histogram_elapsed_time.value(0));

    {
      IntervalAccumulator<testing::fake_clock> _(histogram_elapsed_time);
      testing::fake_clock::advance(std::chrono::milliseconds(10));
    }

    EXPECT_EQ(1, histogram_elapsed_time.value(0));
    EXPECT_EQ(2, histogram_elapsed_time.value(10));

    {
      IntervalAccumulator<testing::fake_clock> _(histogram_elapsed_time);
      testing::fake_clock::advance(std::chrono::seconds(10));
    }

    EXPECT_EQ(1, histogram_elapsed_time.value(0));
    EXPECT_EQ(2, histogram_elapsed_time.value(10));
    EXPECT_EQ(3, histogram_elapsed_time.value(10000));

    {
      IntervalAccumulator<testing::fake_clock, std::chrono::seconds> _(
          histogram_elapsed_time);
      testing::fake_clock::advance(std::chrono::seconds(10));
    }

    EXPECT_EQ(1, histogram_elapsed_time.value(0));
    EXPECT_EQ(3, histogram_elapsed_time.value(10));
    EXPECT_EQ(4, histogram_elapsed_time.value(10000));
  }

} /* namespace */
