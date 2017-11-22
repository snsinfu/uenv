#!/bin/sh -efu
set -efu

. ./common.sh

expected="https://api.example.com/v1"
actual="$(uenv -cf envfiles/sparse.txt sh -uc 'echo "${BASE_URI}"')"
assert_eq "${actual}" "${expected}"

expected="abcdefghijklmnopqrstuvwxyz"
actual="$(uenv -cf envfiles/sparse.txt sh -uc 'echo "${API_TOKEN}"')"
assert_eq "${actual}" "${expected}"
