# Prometheus C++ client library

This is a client library for C++ for [Prometheus](https://prometheus.io).

[![Build Status](https://travis-ci.org/korfuri/client_cpp.svg?branch=master)](https://travis-ci.org/korfuri/client_cpp)

# Getting started

This project uses [Bazel](http://bazel.io) or CMake to build

## Requirements for Bazel
  * [Bazel](http://bazel.io)
  * `apt-get install g++-4.9` or higher
  * `apt-get install libicu-dev` for Unicode support
  * `apt-get install libprotobuf-dev protobuf-compiler` for Protocol Buffers

## Requirements for CMake

  * `apt-get install libicu-dev` for Unicode support
  * `apt-get install libprotobuf-dev protobuf-compiler` for Protocol Buffers

NOTE: Test execution in CMake has not been implemented.

## Requirements for tests when not using Bazel

The Bazel workspace will pull these automatically for you. If you're
not using Bazel, you'll need:

  * [gtest](https://code.google.com/p/googletest/) is used in unit tests.
  * [fake_clock](https://github.com/korfuri/fake_clock) is used in unit tests.

# Usage

TODO :) This is not quite final yet. You are welcome to use the
library but I don't consider it fully stable yet. Contributions (bug
reports, pull requests, suggestions) are welcome!

# Performance

tl;dr: get your own stats by running `bazel test prometheus:benchmark_test`.

On my desktop (quad core i5 @3.4GHz), I get the following results:

| Test                                                          | 1 thread | 10 threads | 100 threads |
|---------------------------------------------------------------|----------|------------|-------------|
| Increment a counter 1M times/thread                           | 23.53ms  | 857ms      | 8881ms      |
| Increment gauge 1M times/thread                               | 23.57ms  | 888ms      | 8955ms      |
| Set a gauge 1M times/thread                                   | 22.95ms  | 194ms      | 1931ms      |
| Create 10k labels/thread                                      |  6.97ms  | 113ms      | 1047ms      |
| Observe 100k values (uniform distribution) in a histogram     | 42.69ms  | 722ms      | 7473ms      |
| Observe 100k values (exponential distribution) in a histogram | 48.78ms  | 783ms      | 7849ms      |

For full details on what the tests are doing, check out `prometheus/benchmark_test.cc`.
