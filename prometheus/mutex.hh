#ifndef PROMETHEUS_MUTEX_HH__
# define PROMETHEUS_MUTEX_HH__

# include <mutex>
# include <shared_mutex>

namespace prometheus { namespace impl {

# ifdef __APPLE__
  using shared_timed_mutex                = std::mutex;
  template <typename T> using shared_lock = std::unique_lock<T>;
# else
  using shared_timed_mutex                = std::shared_timed_mutex;
  template <typename T> using shared_lock = std::shared_lock<T>;
# endif

}} // end of namespace prometheus::impl
#endif // PROMETHEUS_MUTEX_HH__
