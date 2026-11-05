#!/usr/bin/env bash
source "$(dirname "$0")/../../scripts/testlib.sh"
build_sanitized
run_sanitized expected_output.txt 48
build_plain
run_leak_check 48
pass
