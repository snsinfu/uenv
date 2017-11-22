#!/bin/sh -efu
set -efu

. ./common.sh

cd dotenv

expected="aaa"
actual="$(uenv -c sh -uc 'echo "${A}"')"
assert_eq "${actual}" "${expected}"

expected=""
actual="$(uenv -c sh -uc 'echo "${B}"')"
assert_eq "${actual}" "${expected}"

expected="ccc"
actual="$(uenv -c sh -uc 'echo "${C}"')"
assert_eq "${actual}" "${expected}"
