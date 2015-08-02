#include <prometheus/client.hh>

prometheus::Counter<0> events("events_total", "Total number of events");

int main() {
  events.inc();
  return 0;
}
