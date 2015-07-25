cc_library(
    name = "client_cpp_lib",
    srcs = ["client_cpp.cc"],
    hdrs = ["client_cpp.hh",
            "arrayhash.hh",
	    "zipiterator.hh"])

cc_binary(
    name = "client_cpp",
    srcs = ["client_cpp_main.cc"],
    deps = [":client_cpp_lib"])

cc_test(
    name = "client_cpp_test",
    srcs = ["client_cpp_test.cc"],
    deps = [":client_cpp_lib",
            "@gtest//gtest:gtest"])
