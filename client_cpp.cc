#include "client_cpp.hh"

#include <iostream>

int main() {
  UnlabeledMetric<Gauge> blips("blips_total");
  blips.set(4.2);
  blips.output(std::cout);

  LabeledMetric<1, Gauge> blops_by_blarg("blops_by_blarg_total", {"blarg"});
  blops_by_blarg.labels({"grunt"}).set(1.3);
  blops_by_blarg.labels({"blub"}).set(8.6);
  blops_by_blarg.output(std::cout);

  LabeledMetric<2, Gauge> blops_by_blarg_blurg("blops_by_blarg_blurg_total", {"blarg", "blurg"});
  blops_by_blarg_blurg.labels({"grunt", "knix"}).set(1.3);
  blops_by_blarg_blurg.labels({"blub", "knix"}).set(8.6);
  blops_by_blarg_blurg.labels({"grunt", "knux"}).set(1.5);
  blops_by_blarg_blurg.labels({"blub", "knux"}).set(8.2);
  blops_by_blarg_blurg.output(std::cout);

  UnlabeledMetric<Counter> bloops("bloops_total");
  bloops.inc(4.2);
  bloops.inc();
  bloops.output(std::cout);

  return 0;
}
