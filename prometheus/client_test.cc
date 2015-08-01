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

  // TEST_F(ClientCPPTest, LevelUpTest) {
  //   EXPECT_EQ("1.000000", h0a.level_up(0.9));
  //   EXPECT_EQ("1.000000", h0a.level_up(1));
  //   EXPECT_EQ("2.000000", h0a.level_up(1.1));
  //   EXPECT_EQ("+Inf", h0a.level_up(11));
  // }

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
} /* namespace */
