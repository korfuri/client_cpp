/* -*- mode: C++; coding: utf-8-unix -*- */
#include "client.hh"

#include <iostream>

using namespace prometheus;

int main() {
  SetGauge<0> blips("blips_total", "Total number of blips.");
  blips.set(4.2);

  SetGauge<1> blops_by_blarg("blops_by_blarg_total",
                             "Total number of blops for each blarg.",
                             {"blarg"});
  blops_by_blarg.labels({{"grunt"}}).set(1.3);
  blops_by_blarg.labels({{"blub"}}).set(8.6);

  SetGauge<2> blops_by_blarg_blurg(
      "blops_by_blarg_blurg_total",
      "Total number of blops for each (blarg,blurg).", {"blarg", "blurg"});
  blops_by_blarg_blurg.labels({"grunt", "knix"}).set(1.3);
  blops_by_blarg_blurg.labels({"blub", "knix"}).set(8.6);
  blops_by_blarg_blurg.labels(std::array<std::string, 2>({"grunt", "knux"}))
      .set(1.5);
  blops_by_blarg_blurg.labels({"blub", "knux"}).set(8.2);

  Counter<0> bloops("bloops_total", "Total number of bloops.");
  bloops.inc(4.2);
  bloops.inc();

  Histogram<0> blups("blups", "Distribution of blups.");
  blups.record(.5);
  blups.record(5);
  blups.record(10);
  blups.record(10000000);

  Histogram<2> blups_by_blip_blop(
      "blups_by_blip_blop", "Distribution of blups by blip and blop.",
      {"blip", "blop"}, histogram_levels({0, 1, 10, 100, 1000, 10000}));
  blups_by_blip_blop.labels({"a", "b"}).record(.5);
  blups_by_blip_blop.labels({"a", "b"}).record(5);
  blups_by_blip_blop.labels({"a", "c"}).record(10);
  blups_by_blip_blop.labels({"e", "d"}).record(10000000);

  Counter<1> unicode(u8"üñíçøđè-metric",
                     u8"This metric tests üñíçøđè support",
                     {u8"lábel"});
  unicode.labels({u8"valüe"}).inc();
  unicode.labels({u8"🍌"}).inc(2);

  auto v = prometheus::impl::global_registry.output_proto();
  for (auto m : v) {
    std::string s;
    std::cout << prometheus::impl::metricfamily_proto_to_string(m);
    delete m;
  }
  return 0;
}
