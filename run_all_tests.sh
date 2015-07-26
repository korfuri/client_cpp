#!/bin/bash

# If bazel is not in your path, you can invoke this as:
# $ BAZEL=path/to/bazel ./run_all_tests.sh
BAZEL="${BAZEL:-bazel}"

# Using Bazel on Travis is a bit complex right now (see
# https://github.com/korfuri/bazel-travis) so we use the poor man's
# tool: a shell script that runs all tests under various
# configurations.
# Disabled configurations:
#     "-c dbg --config tsan" : requires clang
#     "-c dbg --config msan" : requires clang
for CONFIG in \
    "" \
    "-c dbg" \
    "-c opt" \
    "-c dbg --config asan" \
    "-c dbg --config ubsan"
do
    echo "== Testing with $CONFIG =="
    "${BAZEL?}" test $CONFIG -- //...
done
