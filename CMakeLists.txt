cmake_minimum_required (VERSION 2.8.12)

project ("prometheus-client")

set(PROMETHEUS_CLIENT_CXX_STANDARD --std=c++14)

add_subdirectory(prometheus)
add_subdirectory(integrations/gnu_libmicrohttpd)
add_subdirectory(tests)
