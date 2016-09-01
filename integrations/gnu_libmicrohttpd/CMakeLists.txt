cmake_minimum_required(VERSION 2.8)

project("prometheus-http")
set(PROJECT_VERSION "0.99")
set(PROJECT_DESCRIPTION "Prometheus C++ HTTP server library")

include(GNUInstallDirs)
include(FindPkgConfig)
pkg_check_modules(MHD REQUIRED libmicrohttpd)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

add_library(prometheus-http SHARED prometheus_microhttpd.cc)

target_link_libraries(prometheus-http PUBLIC
  ${MHD_LIBRARIES} prometheus-client)
set_target_properties(prometheus-http PROPERTIES VERSION "0")
set_target_properties(prometheus-http PROPERTIES SOVERSION "0.0.0")

add_executable(example example.c)
target_link_libraries(example prometheus-http)

set(PKG_CONFIG_LIBDIR "\${prefix}/lib")
set(PKG_CONFIG_INCLUDEDIR "\${prefix}/include/")
set(PKG_CONFIG_LIBS "-L\${libdir} -lprometheus-http")
set(PKG_CONFIG_CFLAGS "-I\${includedir}")

configure_file(
  "${CMAKE_SOURCE_DIR}/tools/pkg-config.pc.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc")

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
  DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig")
install(
  TARGETS prometheus-http
  LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}")
install(
  FILES prometheus_microhttpd.h
  DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/prometheus")
