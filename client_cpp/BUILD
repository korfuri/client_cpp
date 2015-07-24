cc_library(
    name = "client_cpp_lib",
    srcs = ["client_cpp.cc"],
    hdrs = ["client_cpp.hh",
            "arrayhash.hh"])

cc_binary(
    name = "client_cpp",
    srcs = ["main.cc"],
    deps = [":client_cpp_lib"])

cc_test(
    name = "client_cpp_test",
    srcs = ["test_client_cpp.cc"],
    deps = [":client_cpp_lib",
            "@gtest//gtest:gtest"])
