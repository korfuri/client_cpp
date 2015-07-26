#include "exceptions.hh"

namespace prometheus {
  namespace err {

    const char* NegativeCounterIncrementException::what() const noexcept {
      return "negative_counter_increment";
    }

    const char* UnsortedLevelsException::what() const noexcept {
      return "unsorted_levels";
    }
  }
}
