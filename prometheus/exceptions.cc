#include "exceptions.hh"

namespace prometheus {
  namespace err {

    const char* NegativeCounterIncrementException::what() const noexcept {
      return "negative_counter_increment";
    }

    const char* UnsortedLevelsException::what() const noexcept {
      return "unsorted_levels";
    }

    const char* InvalidNameException::what() const noexcept {
      return "invalid_name";
    }

    const char* CollectorManagementException::what() const noexcept {
      return "collector_management";
    }
  }
}
