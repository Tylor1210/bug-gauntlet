#!/usr/bin/env bash
source "$(dirname "$0")/../../scripts/testlib.sh"
build_sanitized
run_sanitized expected_output.txt metrics.log
build_plain
run_leak_check metrics.log 200
pass
