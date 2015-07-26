#include "client.hh"

#include <iostream>

using namespace prometheus;

int main() {
  Gauge<0> blips("blips_total", "Total number of blips.");
  blips.set(4.2);

  Gauge<1> blops_by_blarg("blops_by_blarg_total",
                          "Total number of blops for each blarg.", {"blarg"});
  blops_by_blarg.labels({{"grunt"}}).set(1.3);
  blops_by_blarg.labels({{"blub"}}).set(8.6);

  Gauge<2> blops_by_blarg_blurg("blops_by_blarg_blurg_total",
                                "Total number of blops for each (blarg,blurg).",
                                {"blarg", "blurg"});
  blops_by_blarg_blurg.labels({"grunt", "knix"}).set(1.3);
  blops_by_blarg_blurg.labels({"blub", "knix"}).set(8.6);
  blops_by_blarg_blurg.labels(std::array<std::string, 2>({"grunt", "knux"}))
      .set(1.5);
  blops_by_blarg_blurg.labels({"blub", "knux"}).set(8.2);

  Counter<0> bloops("bloops_total", "Total number of bloops.");
  bloops.inc(4.2);
  bloops.inc();

  Histogram<0> blups("blups", "Distribution of blups.",
                     {0, 1, 10, 100, 1000, 10000});
  blups.add(.5);
  blups.add(5);
  blups.add(10);
  blups.add(10000000);

  Histogram<2> blups_by_blip_blop(
      "blups_by_blip_blop", "Distribution of blups by blip and blop.",
      {0, 1, 10, 100, 1000, 10000}, {"blip", "blop"});
  blups_by_blip_blop.add(.5, {"a", "b"});
  blups_by_blip_blop.add(5, {"a", "b"});
  blups_by_blip_blop.add(10, {"a", "c"});
  blups_by_blip_blop.add(10000000, {"e", "d"});

  prometheus::impl::global_registry.output(std::cout);

  return 0;
}
