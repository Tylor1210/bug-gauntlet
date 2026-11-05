#!/usr/bin/env bash
# check.sh: runs every levels/*/test.sh and prints a pass/fail table.
# Usage: scripts/check.sh [level-filter]   e.g. scripts/check.sh 03
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
filter="${1:-}"
printf "%-28s %s\n" "LEVEL" "RESULT"
printf "%-28s %s\n" "-----" "------"
passed=0; total=0
for t in "$ROOT_DIR"/levels/*/test.sh; do
    level="$(basename "$(dirname "$t")")"
    [[ -n "$filter" && "$level" != *"$filter"* ]] && continue
    total=$((total + 1))
    if bash "$t" > /dev/null 2>&1; then
        printf "%-28s %s\n" "$level" "PASS"; passed=$((passed + 1))
    else
        printf "%-28s %s\n" "$level" "fail"
    fi
done
echo
echo "$passed / $total passing"
