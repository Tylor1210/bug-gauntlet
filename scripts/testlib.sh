# testlib.sh: shared helpers sourced by every levels/*/test.sh.
# Builds a level with clang++ directly (no CMake needed), runs it, and checks
# output + sanitizer/leak cleanliness. Depends on: clang++, diff, leaks (macOS).

set -u

LEVEL_DIR="$(cd "$(dirname "$0")" && pwd)"
LEVEL_NAME="$(basename "$LEVEL_DIR")"
ROOT_DIR="$(cd "$LEVEL_DIR/../.." && pwd)"
OUT_DIR="$ROOT_DIR/build/test/$LEVEL_NAME"
mkdir -p "$OUT_DIR"

CXX="${CXX:-clang++}"
BASE_FLAGS=(-std=c++17 -g -O0 -fno-omit-frame-pointer)
SAN_FLAGS=(-fsanitize=address,undefined -fno-sanitize-recover=all)

# WHY: stack-use-after-return detection is off by default on macOS.
export ASAN_OPTIONS="detect_stack_use_after_return=1:abort_on_error=0:halt_on_error=1"
export UBSAN_OPTIONS="print_stacktrace=1:halt_on_error=1"

fail() { echo "FAIL [$LEVEL_NAME]: $*"; exit 1; }
pass() { echo "PASS [$LEVEL_NAME]"; exit 0; }

sources() { ls "$LEVEL_DIR"/*.cpp; }

build_sanitized() {
    "$CXX" "${BASE_FLAGS[@]}" "${SAN_FLAGS[@]}" $(sources) -o "$OUT_DIR/app-san" \
        2> "$OUT_DIR/build-san.log" || { cat "$OUT_DIR/build-san.log"; fail "sanitizer build failed"; }
}

build_plain() {
    "$CXX" "${BASE_FLAGS[@]}" $(sources) -o "$OUT_DIR/app" \
        2> "$OUT_DIR/build.log" || { cat "$OUT_DIR/build.log"; fail "build failed"; }
}

# run_sanitized <expected_file> [args...]: run sanitized build from the level dir,
# require exit 0, no sanitizer report, and stdout identical to expected_file.
run_sanitized() {
    local expected="$1"; shift
    (cd "$LEVEL_DIR" && "$OUT_DIR/app-san" "$@") > "$OUT_DIR/stdout.txt" 2> "$OUT_DIR/stderr.txt"
    local rc=$?
    if grep -qE "ERROR: AddressSanitizer|runtime error:|SUMMARY: (Address|UndefinedBehavior)Sanitizer" "$OUT_DIR/stderr.txt"; then
        echo "--- sanitizer report (first 30 lines) ---"
        head -30 "$OUT_DIR/stderr.txt"
        fail "sanitizer reported an error"
    fi
    [ $rc -eq 0 ] || { tail -20 "$OUT_DIR/stderr.txt"; fail "program exited with status $rc"; }
    if ! diff -u "$LEVEL_DIR/$expected" "$OUT_DIR/stdout.txt" > "$OUT_DIR/diff.txt"; then
        echo "--- output differs from $expected (first 30 lines) ---"
        head -30 "$OUT_DIR/diff.txt"
        fail "wrong output"
    fi
}

# run_leak_check [args...]: run the plain build under macOS `leaks`.
run_leak_check() {
    (cd "$LEVEL_DIR" && MallocStackLogging=1 leaks --atExit -- "$OUT_DIR/app" "$@") > "$OUT_DIR/leaks.txt" 2>&1
    if ! grep -qE "^Process [0-9]+: 0 leaks for 0 total leaked bytes" "$OUT_DIR/leaks.txt"; then
        echo "--- leaks report (first 40 lines) ---"
        grep -E "leaks for|Leak:|ROOT LEAK" "$OUT_DIR/leaks.txt" | head -40
        fail "memory leaked"
    fi
}
