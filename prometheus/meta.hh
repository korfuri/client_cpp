#ifndef PROMETHEUS_CLIENT_META_HH__
#define PROMETHEUS_CLIENT_META_HH__

#include <type_traits>
#include <utility>
#include <cstddef>
#include <tuple>

namespace prometheus { namespace impl {

// Given a Tuple find the first element that has Base as a base class and return
// the index. If no such element is found -1 is returned.
//
// Based on:
// http://stackoverflow.com/questions/33999868/stdtuple-get-item-by-inherited-type

template <typename Base, typename Tuple, int I = 0>
struct index_by_base : std::integral_constant<int, -1> {};

template <typename Base, typename Head, typename... Tail, int I>
struct index_by_base<Base, std::tuple<Head, Tail...>, I>  
    : std::conditional<std::is_base_of<Base, Head>::value
                     , std::integral_constant<int, I>
                     , index_by_base<Base, std::tuple<Tail...>, I+1>
                     >::type
{};

// Given a Tuple instance and a Base type return a reference to the tuple element
// that has Base as a base class.
template <typename Base, typename Tuple>
auto
get_by_base(Tuple&& tuple)
    -> decltype(std::get<index_by_base<Base,
        typename std::decay<Tuple>::type>::value>(std::forward<Tuple>(tuple)))
{
    return std::get<index_by_base<Base,
           typename std::decay<Tuple>::type>::value>(std::forward<Tuple>(tuple));
}

}} // end of namespace prometheus::impl
#endif // PROMETHEUS_CLIENT_META_HH__
