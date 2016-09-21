cmake_minimum_required(VERSION 2.8)

project("prometheus-client")
set(PROJECT_VERSION "0.99")
set(PROJECT_DESCRIPTION "Prometheus C++ client library")

include(GNUInstallDirs)
include(FindPkgConfig)
pkg_check_modules(ICU REQUIRED icu-io icu-i18n icu-uc)
pkg_check_modules(PB REQUIRED protobuf)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

add_library(prometheus-client SHARED
  collector.cc exceptions.cc metrics.cc output_formatter.cc
  registry.cc standard_exports.cc utils.cc values.cc
  proto/metrics.pb.cc)

add_custom_command(
  OUTPUT proto/metrics.pb.cc proto/metrics.pb.h
  COMMAND mkdir -p proto/
  COMMAND protoc -I${CMAKE_SOURCE_DIR}/prometheus/proto
  ${CMAKE_SOURCE_DIR}/prometheus/proto/metrics.proto  --cpp_out=proto/)

target_link_libraries(prometheus-client PUBLIC
  ${ICU_LIBRARIES} ${PB_LIBRARIES})
set_target_properties(prometheus-client PROPERTIES VERSION "0")
set_target_properties(prometheus-client PROPERTIES SOVERSION "0.0.0")

add_executable(client_demo client_demo_main.cc)
target_link_libraries(client_demo prometheus-client)

set(PKG_CONFIG_LIBDIR "\${prefix}/lib")
set(PKG_CONFIG_INCLUDEDIR "\${prefix}/include/")
set(PKG_CONFIG_LIBS "-L\${libdir} -lprometheus-client")
set(PKG_CONFIG_CFLAGS "-I\${includedir}")

configure_file(
  "${CMAKE_SOURCE_DIR}/tools/pkg-config.pc.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc")

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
  DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig")
install(
  TARGETS prometheus-client
  LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}")
install(FILES
  client.hh collector.hh exceptions.hh metrics.hh output_formatter.hh
  registry.hh standard_exports.hh utils.hh values.hh
  DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/prometheus")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/proto/metrics.pb.h"
  DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/prometheus/proto/")
