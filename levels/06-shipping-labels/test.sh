#!/usr/bin/env bash
source "$(dirname "$0")/../../scripts/testlib.sh"
build_sanitized
run_sanitized expected_output.txt contacts.txt
pass
