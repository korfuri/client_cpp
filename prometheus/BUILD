package(
    default_visibility = ["//visibility:private"])

cc_library(
    name = "client_lib",
    srcs = ["client.cc"],
    hdrs = ["client.hh"],
    deps = [
        ":client_lib_impl",
        ":client_exceptions_lib",
    ],
    visibility = ["//visibility:public"])

cc_library(
    name = "client_lib_impl",
    srcs = [
        "metrics.cc",
        "registry.cc",
        "values.cc",
    ],
    hdrs = [
        "metrics.hh",
        "registry.hh",
        "values.hh",
    ],
    deps = [
        "//prometheus/proto:metrics_proto",
        ":client_exceptions_lib",
        "//prometheus/util:container_hash_lib",
        "//prometheus/util:extend_array_lib",
        "//prometheus/util:zipped_iterator_lib",
    ])

cc_library(
    name = "output_formatter_lib",
    srcs = ["output_formatter.cc"],
    hdrs = ["output_formatter.hh"],
    deps = [
        # "@icu//:icu",
        "//prometheus/proto:metrics_proto",
    ],
    copts = [
        "-Iexternal/icu/icu/source/common",
        "-Iexternal/icu/icu/source/i18n",
    ])

cc_library(
    name = "client_exceptions_lib",
    srcs = ["exceptions.cc"],
    hdrs = ["exceptions.hh"],
    visibility = ["//visibility:public"])

cc_binary(
    name = "client_demo",
    srcs = ["client_demo_main.cc"],
    deps = [
        ":client_lib",
        ":output_formatter_lib",
        "@protobuf//:protobuf",
    ])

cc_test(
    name = "client_test",
    srcs = ["client_test.cc"],
    deps = [
        ":client_lib",
        "@gtest//gtest:gtest",
        "@gtest//gtest:gtest_main",
    ],
    size = "small",
    timeout = "short")

cc_test(
    name = "client_concurrent_test",
    srcs = ["client_concurrent_test.cc"],
    deps = [
        ":client_lib",
        "@gtest//gtest:gtest",
        "@gtest//gtest:gtest_main",
    ],
    size = "small")

sh_test(
    name = "client_integration_test",
    srcs = ["client_integration_test.sh"],
    data = [
        ":client_demo",
        "testdata/ref.txt",
    ],
    size = "small",
    timeout = "short")

cc_test(
    name = "output_formatter_test",
    srcs = ["output_formatter_test.cc"],
    deps = [
        ":output_formatter_lib",
        "@gtest//gtest:gtest",
        "@gtest//gtest:gtest_main",
    ],
    size = "small",
    timeout = "short")
