package(
    default_visibility = ["//visibility:private"])

cc_library(
    name = "client_lib",
    srcs = [
        "client.cc",
        "output_formatter.cc",
        "registry.cc",
        "values.cc",
    ],
    hdrs = [
        "client.hh",
        "output_formatter.hh",
        "metrics.hh",
        "registry.hh",
        "values.hh",
    ],
    deps = [
        "//prometheus/util:zipped_iterator_lib",
        "//prometheus/util:container_hash_lib",
    ],
    visibility = ["//visibility:public"])

cc_binary(
    name = "client_demo",
    srcs = ["client_demo_main.cc"],
    deps = [":client_lib"])

cc_test(
    name = "client_test",
    srcs = ["client_test.cc"],
    deps = [
        ":client_lib",
        "@gtest//gtest:gtest",
    ])

sh_test(
    name = "client_integration_test",
    srcs = ["client_integration_test.sh"],
    data = [
        ":client_demo",
        "testdata/ref.txt",
    ])
