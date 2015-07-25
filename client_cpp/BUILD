package(
    default_visibility = ["//visibility:private"])

cc_library(
    name = "client_cpp_lib",
    srcs = [
        "client_cpp.cc",
        "output_formatter.cc",
        "registry.cc",
        "values.cc",
    ],
    hdrs = [
        "client_cpp.hh",
        "output_formatter.hh",
        "metrics.hh",
        "registry.hh",
        "values.hh",
    ],
    deps = [
        "//client_cpp/util:zipped_iterator_lib",
        "//client_cpp/util:container_hash_lib",
    ],
    visibility = ["//visibility:public"])

cc_binary(
    name = "client_cpp",
    srcs = ["client_cpp_main.cc"],
    deps = [":client_cpp_lib"])

cc_test(
    name = "client_cpp_test",
    srcs = ["client_cpp_test.cc"],
    deps = [
        ":client_cpp_lib",
        "@gtest//gtest:gtest",
    ])

sh_test(
    name = "client_cpp_integration_test",
    srcs = ["client_cpp_integration_test.sh"],
    data = [
        ":client_cpp",
        "testdata/ref.txt",
    ])
