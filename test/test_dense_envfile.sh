#!/bin/sh -efu
set -efu

. ./common.sh

expected="example.com:1234"
actual="$(uenv -cf envfiles/dense.txt sh -uc 'echo "${SERVER}"')"
assert_eq "${actual}" "${expected}"

expected="johndoe"
actual="$(uenv -cf envfiles/dense.txt sh -uc 'echo "${USERNAME}"')"
assert_eq "${actual}" "${expected}"

expected="correct horse battery staple"
actual="$(uenv -cf envfiles/dense.txt sh -uc 'echo "${PASSWORD}"')"
assert_eq "${actual}" "${expected}"
