#ifndef PROMETHEUS_ARRAYHASH_HH__
#define PROMETHEUS_ARRAYHASH_HH__

#include <functional>
#include <type_traits>

namespace prometheus {
namespace util {

template <class Container, template <class> class ItemHash = std::hash>
struct ContainerHash {
  typedef Container argument_type;
  typedef typename std::remove_reference<decltype(*Container().begin())>::type
      value_type;
  typedef std::hash<value_type> hash_type;
  typedef typename hash_type::result_type result_type;

  result_type operator()(const Container& container) const {
    hash_type hasher;
    result_type hash = 0;
    for (const auto& it : container) {
      hash = hasher(it) ^ hash << 1;
    }
    return hash;
  }
};

template <class Container>
struct ContainerEq {
  bool operator()(const Container& lhs, const Container& rhs) const {
    typename Container::const_iterator itr = rhs.begin();
    for (const auto& itl : lhs) {
      if (itr == rhs.end() || !(itl == *itr)) return false;
      itr++;
    }
    return itr == rhs.end();
  }
};
}
}

#endif /* PROMETHEUS_ARRAYHASH_HH__ */
