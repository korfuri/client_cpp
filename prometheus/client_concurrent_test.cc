#include "client.hh"
#include <gtest/gtest.h>
#include <list>
#include <thread>

namespace {

class ClientConcurrentTest : public ::testing::Test {};

  TEST_F(ClientConcurrentTest, CounterTest) {
    std::list<std::thread> l;
    for (int i = 0; i < 40; ++i) {
      
    }
  }

}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
