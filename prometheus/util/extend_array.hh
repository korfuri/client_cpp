#ifndef PROMETHEUS_UTIL_EXTEND_ARRAY_HH__
#define PROMETHEUS_UTIL_EXTEND_ARRAY_HH__

#include <array>

namespace prometheus {
namespace util {

template <unsigned long N, typename T = std::string>
std::array<T, N + 1> extend_array(std::array<T, N> const& ar, T const& v) {
  std::array<T, N + 1> ret;
  std::copy(ar.begin(), ar.end(), ret.begin());
  ret[ret.size() - 1] = v;
  return ret;
}
}
}

#endif
