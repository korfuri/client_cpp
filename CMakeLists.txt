cmake_minimum_required (VERSION 2.8)

project ("prometheus-client")

add_definitions(-std=c++0x)

add_subdirectory(prometheus)
add_subdirectory(integrations/gnu_libmicrohttpd)
add_subdirectory(tests)
