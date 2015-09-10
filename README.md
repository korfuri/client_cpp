# Prometheus C++ client library

This is a client library for C++ for [Prometheus](https://prometheus.io).

[![Build Status](https://travis-ci.org/korfuri/client_cpp.svg?branch=master)](https://travis-ci.org/korfuri/client_cpp)

# Getting started

This project uses [Bazel](http://bazel.io) to build. I welcome
contributions to make it work on other build systems, as Bazel is kind
of special in the way it interacts with non-Bazel projects.

## Requirements
  * [Bazel](http://bazel.io)
  * `apt-get install g++-4.9` or higher
  * `apt-get install libicu-dev` for Unicode support
  * `apt-get install libprotobuf-dev protobuf-compiler` for Protocol Buffers

## Requirements when not using Bazel

The Bazel workspace will pull these automatically for you. If you're
not using Bazel, you'll need:

  * [gtest](https://code.google.com/p/googletest/) is used in unit tests.
  * [fake_clock](https://github.com/korfuri/fake_clock) is used in unit tests.

# Usage

TODO :) This is not quite final yet. You are welcome to use the
library but I don't consider it fully stable yet. Contributions (bug
reports, pull requests, suggestions) are welcome!
