cmake_minimum_required(VERSION 2.8.12)

project("prometheus-client")
set(PROJECT_VERSION "0.99")
set(PROJECT_DESCRIPTION "Prometheus C++ client library")
set(CMAKE_MACOSX_RPATH true)

include(GNUInstallDirs)
include(FindPkgConfig)
if (APPLE)
  # Find homebrew's libicu on Mac OS although it is hidden pretty well
  set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/local/opt/icu4c/lib/pkgconfig")
endif()
pkg_check_modules(ICU REQUIRED icu-io icu-i18n icu-uc)
pkg_check_modules(PB REQUIRED protobuf)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${ICU_INCLUDE_DIRS})
link_directories(${ICU_LIBRARY_DIRS})

add_library(prometheus-client SHARED
  collector.cc exceptions.cc metrics.cc output_formatter.cc
  registry.cc standard_exports.cc utils.cc values.cc
  proto/metrics.pb.cc)

add_custom_command(
  OUTPUT proto/metrics.pb.cc proto/metrics.pb.h
  COMMAND mkdir -p proto/
  COMMAND protoc -I${CMAKE_SOURCE_DIR}/prometheus/proto
  ${CMAKE_SOURCE_DIR}/prometheus/proto/metrics.proto  --cpp_out=proto/)

target_link_libraries(prometheus-client
                      PUBLIC ${PB_LIBRARIES}
                      PRIVATE ${ICU_LIBRARIES})
set_target_properties(prometheus-client PROPERTIES
                      VERSION "0"
                      SOVERSION "0.0.0")
target_compile_options(prometheus-client PRIVATE ${PROMETHEUS_CLIENT_CXX_STANDARD})

add_executable(client_demo client_demo_main.cc)
target_link_libraries(client_demo prometheus-client)
target_compile_options(client_demo PRIVATE ${PROMETHEUS_CLIENT_CXX_STANDARD})

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
