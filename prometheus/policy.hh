#ifndef PROMETHEUS_POLICY_HH__
# define PROMETHEUS_POLICY_HH__

#include "meta.hh"

namespace prometheus {

namespace impl {

// This machinery deals with fishing out the labels passed by the user. The
// entry point is the labels(...) function below.

// A tag type to identify label_arrays of arbitrary size.
struct label_array_tag {};

// A type to tag and temporarily hold the labels.
template <size_t N> 
struct label_array : std::array<std::string, N>
                   , label_array_tag
{
  static constexpr size_t size = N;
  label_array(std::array<std::string, N> const& labels) :
      std::array<std::string, N>(labels) {}
};

// The get_dims<Args...> meta function fishes the label_array type from its
// arguments and returns its size. In case Args does not contain a label_array
// the result is zero.
template <typename Tuple, int Pos, bool>
struct dims_aux : std::integral_constant<size_t, 0>::type {};

template <typename Tuple, int Pos>
struct dims_aux<Tuple, Pos, true> :
  std::integral_constant<size_t, std::tuple_element<Pos, Tuple>::type::size>::type {};

template <typename... Args>
class get_dims {
  using arg_tuple = std::tuple<Args...>;
  static constexpr int labels_index = index_by_base<label_array_tag, arg_tuple>::value;
public:
  static constexpr size_t value = dims_aux<arg_tuple, labels_index, labels_index >= 0>::value;
};

} // end of namespace impl

// User API function to pass labels to metrics like so:
// auto c = makeCounter("foo", "foo counter", labels("size", "kind"));
template <typename... Args>
impl::label_array<sizeof...(Args)>
labels(Args const& ... args) {
  return impl::label_array<sizeof...(Args)>({args...});
}

} // end of namespace prometheus

#endif // PROMETHEUS_POLICY_HH__

