#ifndef PROMETHEUS_ZIPITERATOR_HH__
#define PROMETHEUS_ZIPITERATOR_HH__

#include <functional>
#include <utility>

namespace prometheus {
  namespace util {

    template <typename Tl, typename Tr>
    class ZippedIterator {
     public:
      ZippedIterator(Tl const &l, Tr const &r) : l_(l), r_(r) {}

      ZippedIterator &operator++() {
        ++l_;
        ++r_;
        return *this;
      }
      // Don't even bother providing operator++(int). It's useless and it
      // creates copies.

      // This is a pair of references on the values pointed by both the
      // left and right iterators.
      typedef decltype(
          std::make_pair(std::ref(*(Tl())), std::ref(*(Tr())))) pair;

      pair operator*() { return std::make_pair(std::ref(*l_), std::ref(*r_)); }

      pair operator*() const {
        return std::make_pair(std::ref(*l_), std::ref(*r_));
      }

      bool operator==(ZippedIterator const &rhs) const {
        // Iterators should always be in sync. One case where the
        // iterators may not be equal at the same time is if they refer to
        // collections of various lengths, one will reach the end iterator
        // before the other. To guard against that we consider zipped
        // iterators equal if either their left or their right iterator
        // are equal.
        return (l_ == rhs.l_) || (r_ == rhs.r_);
      }

      bool operator!=(ZippedIterator const &rhs) const {
        return !(*this == rhs);
      }

     private:
      Tl l_;
      Tr r_;
    };

    template <typename Tl, typename Tr>
    ZippedIterator<Tl, Tr> zip_iterators(Tl const &l, Tr const &r) {
      return ZippedIterator<Tl, Tr>(l, r);
    }
  }
}

#endif
