cmake_minimum_required (VERSION 2.8.12)

project ("prometheus-client")
enable_testing()

set(PROMETHEUS_CLIENT_CXX_STANDARD --std=c++14)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/external/gtest/CMakeLists.txt)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/gtest)
  set(PROMETHEUS_HAS_LOCAL_GTEST true)
  set(GTEST_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/gtest/googletest/include)
  set(PROMETHEUS_FAKE_CLOCK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/fake_clock)
  add_library(fake_clock external/fake_clock/fake_clock.cc)
  target_compile_options(fake_clock PRIVATE ${PROMETHEUS_CLIENT_CXX_STANDARD})
endif()

add_subdirectory(prometheus)
add_subdirectory(integrations/gnu_libmicrohttpd)
add_subdirectory(tests)
