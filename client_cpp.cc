#include "client_cpp.hh"

#include <iostream>

using namespace prometheus;

int main() {
  Gauge<0> blips("blips_total");
  blips.set(4.2);
  blips.output(std::cout);

  Gauge<1> blops_by_blarg("blops_by_blarg_total", {"blarg"});
  blops_by_blarg.labels({"grunt"}).set(1.3);
  blops_by_blarg.labels({"blub"}).set(8.6);
  blops_by_blarg.output(std::cout);

  Gauge<2> blops_by_blarg_blurg("blops_by_blarg_blurg_total", {"blarg", "blurg"});
  blops_by_blarg_blurg.labels({"grunt", "knix"}).set(1.3);
  blops_by_blarg_blurg.labels({"blub", "knix"}).set(8.6);
  blops_by_blarg_blurg.labels(std::array<std::string, 2>({"grunt", "knux"})).set(1.5);
  blops_by_blarg_blurg.labels({"blub", "knux"}).set(8.2);
  blops_by_blarg_blurg.output(std::cout);

  Counter<0> bloops("bloops_total");
  bloops.inc(4.2);
  bloops.inc();
  bloops.output(std::cout);

  Histogram blups("blups", {0, 1, 10, 100, 1000, 10000});
  blups.add(.5);
  blups.add(5);
  blups.add(10);
  blups.add(10000000);
  blups.output(std::cout);

  return 0;
}
