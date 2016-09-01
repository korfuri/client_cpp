cmake_minimum_required(VERSION 2.8)

project("prometheus-client-tests")
set(PROJECT_VERSION "0.99")
set(PROJECT_DESCRIPTION "Prometheus C++ client tests")

include_directories(${CMAKE_SOURCE_DIR})

add_executable(without_bazel without_bazel.cpp)
target_link_libraries(without_bazel prometheus-client)
