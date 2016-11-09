
#include <gtest/gtest.h>

#include "meta.hh"

// Strange tests here. We compute things at compile time, then run a
// program to dump the results.
namespace {

struct A {};
struct B : A {};
struct C {};
struct D {};
struct E {};

TEST(MetaTest, TestTupleIndexByBase) {
  using prometheus::impl::index_by_base;
  using t1 = std::tuple<C, B, D>;
  ASSERT_EQ(1,  (index_by_base<A, t1>::value));
  ASSERT_EQ(1,  (index_by_base<B, t1>::value));
  ASSERT_EQ(0,  (index_by_base<C, t1>::value));
  ASSERT_EQ(2,  (index_by_base<D, t1>::value));
  ASSERT_EQ(-1, (index_by_base<E, t1>::value));
}

} // end of anonymous namespace
