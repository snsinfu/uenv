#!/bin/sh -efu
set -efu

. common.sh

expected="PATH=$(getconf PATH)"
actual="$(uenv -c env)"
assert_eq "${actual}" "${expected}"
