#!/bin/bash

set -eu

OUT="$(mktemp -p "${TEST_TMPDIR?}")"
SORTED_OUT="$(mktemp -p "${TEST_TMPDIR?}")"
SORTED_REF="$(mktemp -p "${TEST_TMPDIR?}")"

REF="prometheus/testdata/ref.txt"

prometheus/client_demo > "${OUT?}"
sort "${OUT?}" > "${SORTED_OUT?}"
sort "${REF?}" > "${SORTED_REF?}"

echo "Original output can be found at ${OUT?}"
echo "Sorted output can be found at ${SORTED_OUT?}"
echo "Original reference can be found at ${REF}"
echo "Sorted output can be found at ${SORTED_REF?}"
echo

exec diff "${SORTED_OUT?}" "${SORTED_REF?}"
