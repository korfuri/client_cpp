package(
    default_visibility = ["//visibility:private"])

cc_library(
    name = "prometheus_client_lib",
    srcs = [
        "collector.cc",
        "exceptions.cc",
        "metrics.cc",
        "metrics.hh",
        "registry.cc",
        "registry.hh",
	"utils.cc",
        "values.cc",
        "values.hh",
    ],
    hdrs = [
        "collector.hh",
        "client.hh",
        "exceptions.hh",
	"utils.hh",
    ],
    deps = [
        "//prometheus/proto:stubs",
        "//prometheus/proto:metrics_proto",
        "//prometheus/util:container_hash_lib",
        "//prometheus/util:extend_array_lib",
        "//prometheus/util:zipped_iterator_lib",
    ],
    visibility = ["//visibility:public"])

cc_library(
    name = "prometheus_output_formatter_lib",
    srcs = ["output_formatter.cc"],
    hdrs = ["output_formatter.hh"],
    deps = [
        "//prometheus/proto:metrics_proto",
    ])

cc_binary(
    name = "client_demo",
    srcs = ["client_demo_main.cc"],
    deps = [
        ":prometheus_client_lib",
        ":prometheus_output_formatter_lib",
        "@protobuf//:protobuf",
    ])

cc_test(
    name = "client_test",
    srcs = ["client_test.cc"],
    deps = [
        ":prometheus_client_lib",
        "@fake_clock//:fake_clock_lib",
        "@gtest//gtest:gtest_main",
    ],
    size = "small",
    timeout = "short")

cc_test(
    name = "client_concurrent_test",
    srcs = ["client_concurrent_test.cc"],
    deps = [
        ":prometheus_client_lib",
        "@gtest//gtest:gtest_main",
    ],
    size = "small")

cc_test(
    name = "benchmark_test",
    srcs = ["benchmark_test.cc"],
    deps = [
        ":prometheus_client_lib",
        ":prometheus_output_formatter_lib",
        "@gtest//gtest:gtest",
    ],
    size = "large",
    timeout = "long",
    # Don't run this test when testing //...
    tags = ["manual"])

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
        ":prometheus_output_formatter_lib",
        "@gtest//gtest:gtest",
        "@gtest//gtest:gtest_main",
    ],
    size = "small",
    timeout = "short")
