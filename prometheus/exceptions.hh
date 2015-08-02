#ifndef PROMETHEUS_EXCEPTIONS_HH__
#define PROMETHEUS_EXCEPTIONS_HH__

#include <exception>

namespace prometheus {
  namespace err {

    class InvalidNameException : public std::exception {
      // A metric or a label was created with an invalid name.
      virtual const char* what() const noexcept;
    };

    class NegativeCounterIncrementException : public std::exception {
      // A Counter was incremented (inc()) with a negative value.
      // Counters can never be decremented: to decrement a value, use
      // a Gauge.
      virtual const char* what() const noexcept;
    };

    class UnsortedLevelsException : public std::exception {
      // A Histogram was created with levels that are not in a
      // strictly increasing order.
      virtual const char* what() const noexcept;
    };
  }
}

#endif
