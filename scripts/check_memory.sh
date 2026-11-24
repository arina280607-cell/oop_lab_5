#!/usr/bin/env bash
set -euo pipefail

project_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
cd -- "$project_dir"
mkdir -p build-memory

# Use the same Google Test suite even when Linux has no CMake/GTest package.
gtest_dir="${LAB05_GTEST_SOURCE:-build-memory/googletest-1.17.0/googletest}"
if [[ ! -f "$gtest_dir/src/gtest-all.cc" ]]; then
    if [[ -n "${LAB05_GTEST_SOURCE:-}" ]]; then
        echo "Google Test sources not found: $gtest_dir" >&2
        exit 1
    fi
    archive="build-memory/googletest-1.17.0.tar.gz"
    if [[ ! -f "$archive" ]]; then
        curl --fail --location \
            https://codeload.github.com/google/googletest/tar.gz/refs/tags/v1.17.0 \
            --output "$archive"
    fi
    tar -xzf "$archive" -C build-memory
fi

sources=(src/employee.cpp src/employee_tree.cpp src/selection.cpp src/criterion.cpp)
flags=(-std=c++20 -Wall -Wextra -Wpedantic -Werror -g -O1
       -fsanitize=address,undefined -fno-omit-frame-pointer -I src)

g++ "${flags[@]}" -pthread -I "$gtest_dir/include" -I "$gtest_dir" \
    tests/test_lab05.cpp "${sources[@]}" \
    "$gtest_dir/src/gtest-all.cc" "$gtest_dir/src/gtest_main.cc" \
    -o build-memory/lab_05_tests

g++ "${flags[@]}" src/main.cpp "${sources[@]}" -o build-memory/lab_05

export ASAN_OPTIONS=detect_leaks=1:halt_on_error=1
export UBSAN_OPTIONS=halt_on_error=1

./build-memory/lab_05_tests
./build-memory/lab_05
