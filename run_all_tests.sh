#!/bin/bash

set +eux 

# If bazel is not in your path, you can invoke this as:
# $ BAZEL=path/to/bazel ./run_all_tests.sh
BAZEL="${BAZEL:-bazel}"

# Using Bazel on Travis is a bit complex right now (see
# https://github.com/korfuri/bazel-travis) so we use the poor man's
# tool: a shell script that runs all tests under various
# configurations.
# Disabled configurations:
#     "-c dbg --config tsan" : protoc segfaults under tsan
#     "-c dbg --config msan" : requires clang
for CONFIG in \
    "-c dbg" \
    "-c opt" \
    "-c dbg --config asan" \
    "-c dbg --config ubsan"
do
    echo "[.] Testing with $CONFIG"
    "${BAZEL?}" test $CONFIG -- //...
done

echo "[.] Testing in fastbuild mode"

# Run all tests under fastbuild mode
"${BAZEL?}" build -- //prometheus:all
"${BAZEL?}" test -- //prometheus:all

# Use the newly built libs to try to build something outside of Bazel.
# We pass a few linker flags to pretend the binaries installed.
echo "[.] Building a binary that uses our lib."
make -C tests without_bazel
echo "[.] Running that binary."
LD_LIBRARY_PATH=$(pwd)/bazel-bin/prometheus:$(pwd)/bazel-bin/prometheus/proto:$(pwd)/bazel-bin/external/protobuf:$LD_LIBRARY_PATH tests/without_bazel && echo "[.] PASS" || echo "[!] FAILED"
