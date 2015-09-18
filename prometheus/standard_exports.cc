#include "collector.hh"
#include "registry.hh"
#include "standard_exports.hh"
#include "prometheus/proto/metrics.pb.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <iostream>

namespace prometheus {
  namespace impl {

    struct ProcSelfStatReader {

      ProcSelfStatReader() {
        std::ifstream in("/proc/self/stat");
        // Extracts all fields from /proc/self/stat. This assumes a
        // Linux 2.6 distro (importantly, times are expressed in ticks
        // and not in jffies). Not all fields are actually used for
        // exports.
        in
          >> pid >> filename >> state >> ppid >> pgrp
          >> session >> ttynr >> tpgid >> flags >> minflt
          >> cminflt >> majflt >> cmajflt >> utime >> stime
          >> cutime >> cstime >> priority >> nice >> numthreads
          >> itrealvalue >> starttime >> vsize >> rss;
      }

      char state;
      int pid, ppid, pgrp, session, ttynr, tpgid;
      unsigned int flags;
      unsigned long int minflt, cminflt, majflt, cmajflt, utime, stime;
      long int cutime, cstime, priority, nice, numthreads, itrealvalue;
      unsigned long long int starttime;
      unsigned long int vsize;
      long int rss;
      std::string filename;
    };

    struct ProcStatReader {
      ProcStatReader() {
        std::ifstream in("/proc/stat");
        std::string line;
        while (in.good()) {
          std::getline(in, line);
          if (line.compare(0, 6, "btime ") == 0) {
            btime = std::stoi(line.substr(6));
          }
        }
      }

      long int btime;
    };

    struct ProcSelfFdReader {
      class OpenDirException {};

      ProcSelfFdReader() : num_open_files(0) {
        DIR* dir = opendir("/proc/self/fd");
        if (dir == nullptr) {
          throw OpenDirException();
        }

        errno = 0;
        while (readdir(dir)) {
          ++num_open_files;
        }
        closedir(dir);
        if (errno) {
          throw OpenDirException();
        }
      }

      rlim_t num_open_files;
    };

    struct ProcSelfLimitsReader {
      ProcSelfLimitsReader() {
        std::ifstream in("/proc/self/limits");
        std::string line;
        while (in.good()) {
          std::getline(in, line);
          if (line.compare(0, 14, "Max open files") == 0) {
            max_open_files = std::stoi(line.substr(14));
          }
        }
      }

      rlim_t max_open_files;
    };

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
        ProcSelfStatReader pss;
        ProcStatReader ps;
        ProcSelfFdReader psfd;
        ProcSelfLimitsReader psl;

        set_gauge(l, "process_virtual_memory_bytes", "Virtual memory size in bytes (vsize)", pss.vsize);
        set_gauge(l, "process_resident_memory_bytes", "Resident memory size in bytes (rss)", pss.rss * pagesize_);
        set_gauge(l, "process_start_time_seconds", "Start time of the process since unix epoch in seconds.", pss.starttime / ticks_per_ms_ + ps.btime);
        set_gauge(l, "process_cpu_seconds_total", "Total user and system CPU time spent in seconds.", (double)(pss.utime + pss.stime) / ticks_per_ms_);
        set_gauge(l, "process_open_fds", "Number of open file descriptors.", psfd.num_open_files);
        set_gauge(l, "process_max_fds", "Maximum number of open file descriptors.", psl.max_open_files);
        return l;
      }
    };
  } /* namespace impl */

  impl::ProcessCollector* global_process_collector = nullptr;

  void install_process_exports() {
    global_process_collector = new impl::ProcessCollector();
  }
} /* namespace prometheus */
