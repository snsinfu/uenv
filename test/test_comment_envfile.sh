#!/bin/sh -efu
set -efu

. common.sh

expected="PATH=$(getconf PATH)"
actual="$(uenv -cf envfiles/comment_only.txt env)"
assert_eq "${actual}" "${expected}"
