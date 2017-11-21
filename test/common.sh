assert_eq() {
    if [ x"$1" != x"$2" ]; then
        echo "  Actual:   \"$1\""
        echo "  Expected: \"$2\""
        return 1
    fi
}
