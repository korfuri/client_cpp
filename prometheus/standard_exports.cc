#include "collector.hh"
#include "registry.hh"
#include "standard_exports.hh"
#include "prometheus/proto/metrics.pb.h"
#include <unistd.h>
#include <fstream>
#include <iostream>

namespace prometheus {
  namespace impl {

    class ProcessCollector : public ICollector {
    private:
      // Convenience function to add a gauge to the list of
      // MetricFamilies and set its name/help/type and one value.
      static void set_gauge(std::list<MetricFamily*>& l,
                            std::string const& name,
                            std::string const& help,
                            double value) {
        MetricFamily* mf = new MetricFamily();
        mf->set_name(name);
        mf->set_help(help);
        mf->set_type(::prometheus::client::MetricType::GAUGE);
        mf->add_metric()->mutable_gauge()->set_value(value);
        l.push_back(mf);
      }

      const double pagesize_;
      const double ticks_per_ms_;

    public:
      ProcessCollector() :
        pagesize_(sysconf(_SC_PAGESIZE)),
        ticks_per_ms_(sysconf(_SC_CLK_TCK)) {
        global_registry.register_collector(this);
      }

      ~ProcessCollector() {
        global_registry.unregister_collector(this);
      }

      std::list<MetricFamily*> collect() const {
        std::list<MetricFamily*> l;
        std::ifstream in("/proc/self/stat");
        int pid;
        std::string filename;

        // Extracts all fields from /proc/self/stat. This assumes a
        // Linux 2.6 distro (importantly, times are expressed in ticks
        // and not in jffies). Not all fields are actually used for
        // exports.
        char state;
        int ppid, pgrp, session, ttynr, tpgid;
        unsigned int flags;
        unsigned long int minflt, cminflt, majflt, cmajflt, utime, stime;
        long int cutime, cstime, priority, nice, numthreads, itrealvalue;
        unsigned long long int starttime;
        unsigned long int vsize;
        long int rss;
        in
          >> pid >> filename >> state >> ppid >> pgrp
          >> session >> ttynr >> tpgid >> flags >> minflt
          >> cminflt >> majflt >> cmajflt >> utime >> stime
          >> cutime >> cstime >> priority >> nice >> numthreads
          >> itrealvalue >> starttime >> vsize >> rss;

        set_gauge(l, "process_virtual_memory_bytes", "Virtual memory size in bytes (vsize)", vsize);
        set_gauge(l, "process_resident_memory_bytes", "Resident memory size in bytes (rss)", rss * pagesize_);
        int btime = 0; // TODO(korfuri): Get this from /proc/stat
        set_gauge(l, "process_start_time_seconds", "Start time of the process since unix epoch in seconds.", starttime / ticks_per_ms_ + btime);
        set_gauge(l, "process_cpu_seconds_total", "Total user and system CPU time spent in seconds.", (double)(utime + stime) / ticks_per_ms_);
        // TODO(korfuri): process_open_fds
        // TODO(korfuri): process_max_fds
        return l;
      }
    };
  } /* namespace impl */

  impl::ProcessCollector* global_process_collector = nullptr;

  void install_process_exports() {
    global_process_collector = new impl::ProcessCollector();
  }
} /* namespace prometheus */
