#!/bin/sh -efu
set -efu

. ./common.sh

expected="PATH=$(getconf PATH)"
actual="$(uenv -cf envfiles/empty.txt env)"
assert_eq "${actual}" "${expected}"
