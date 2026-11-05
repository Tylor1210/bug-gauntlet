# bug-gauntlet

A hands-on course in traditional C++ debugging. Every level is a small, realistic
program with exactly one bug and a ticket describing only the symptom. Your job:
reproduce it, find the root cause with the debugger and sanitizers, fix it, and
explain why it happened.

## Layout

```
levels/NN-name/
  TICKET.md            the bug report: symptoms only
  *.cpp, *.h           the program
  <data file>          input the program reads by default
  expected_output.txt  what correct output looks like
  test.sh              exits 0 only when the bug is truly fixed
scripts/check.sh       runs every level's test, prints a table
incidents/app/         Phase 2: the link page service (production incidents later)
PROGRESS.md            your log: status, time, one-line lesson
```

## How to play a level

1. Read `TICKET.md`. Don't read the code yet.
2. Reproduce the symptom: build and run the program yourself.
3. Form a hypothesis, then use the tools below to confirm or kill it.
4. Fix it, then run `bash levels/NN-name/test.sh` until it prints `PASS`.
5. Explain to yourself (or the tutor) **why** the bug caused exactly that symptom.
6. Fill in your row in `PROGRESS.md`.

Rules for yourself: stay in lldb on levels 01-08 (no print debugging), and don't
"fix" a level by deleting the feature or changing the test or data.

## Building

From the repo root:

```bash
cmake -S . -B build && cmake --build build                                   # normal debug build
cmake -S . -B build-asan -DGAUNTLET_SANITIZE=ON && cmake --build build-asan  # ASan + UBSan
```

Binaries land in `build/bin/<level-name>` and `build-asan/bin/<level-name>`. Run
them from the level directory so they find their input file:

```bash
cd levels/01-top-sellers && ../../build/bin/01-top-sellers
```

`test.sh` doesn't need CMake: it compiles with `clang++` directly into `build/test/`.

### VS Code

- **Cmd+Shift+B** runs `build`. Other tasks: `build (sanitizers)`, `test current level`, `check all levels`.
- **Run and Debug** (F5): pick `Debug level` or `Debug level (sanitizers)`, then pick the level.
  CodeLLDB stops on crashes and sanitizer reports, so you land in the faulting frame.

---

## Cheat sheet

### lldb

Start: `lldb ./app -- arg1 arg2`, then `run` (or `r`). In VS Code, the debug
console accepts the same commands.

| Goal | Command |
|---|---|
| Break at a function / line | `b Foo::bar` / `b file.cpp:42` |
| Conditional breakpoint | `b file.cpp:42 -c 'i == 5'` / `br mod -c 'x > 100' 1` |
| Break when an error is about to be reported | `b __asan::ReportGenericError` / `b __ubsan_on_report` / `b malloc_error_break` |
| List / delete / disable breakpoints | `br list` / `br del 2` / `br dis 2` |
| Step over / into / out | `n` / `s` / `finish` |
| Continue | `c` |
| Backtrace | `bt` (all threads: `bt all`) |
| Move between frames | `up` / `down` / `f 3` |
| Show locals / args | `v` (`frame variable`) |
| Evaluate an expression | `p expr` / `p/x expr` (hex) / `p *ptr` / `p arr[3]` |
| Print n elements behind a pointer | `parray 8 ptr` |
| Raw memory | `memory read -c 64 -f x ptr` (`x/16xw ptr` also works) |
| Watch a variable for writes | `watchpoint set variable obj.field` (`w s v`) |
| Watch an address | `watchpoint set expression -s 4 -- &obj.field` |
| Conditional watchpoint | `watchpoint modify -c 'field > 10' 1` |
| Run until a line | `thread until 57` |
| Re-run from start | `process launch` (or `r` again) |
| Threads | `thread list` / `thread select 3` |
| Disassembly around pc | `di -p` |

Tip: write commands into a file and `command source cmds.lldb` to replay a session.

### AddressSanitizer + UBSan

Build with `-fsanitize=address,undefined -fno-omit-frame-pointer -g` (the
`build-asan` CMake config and every `test.sh` already do this).

Reading a report:
1. **First line**: the error type (`heap-buffer-overflow`, `stack-buffer-overflow`,
   `heap-use-after-free`, `stack-use-after-return`, `attempting double-free`, ...).
2. **`READ/WRITE of size N`** + the first stack: where the bad access happened.
3. **`freed by thread ... here`** / **`previously allocated by`**: the history of that memory.
4. **`Address ... is located N bytes inside/after ...`**: which object, and how far out of bounds.
5. UBSan reports look like `file.cpp:LINE:COL: runtime error: ...`.

Useful environment options:

```bash
ASAN_OPTIONS=detect_stack_use_after_return=1   # off by default on macOS
ASAN_OPTIONS=abort_on_error=1                  # abort so a debugger stops right there
UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1
```

Combine options with `:`. ASan also fills new heap memory with `0xbe` bytes, so
values like `0xbebebebe` in a sanitizer build mean "nobody wrote this".

Note: LeakSanitizer (`detect_leaks`) is not supported on macOS. Use `leaks`.

### leaks (macOS)

Build **without** ASan, then:

```bash
leaks --atExit -- ./app args                          # leak count + sizes at exit
MallocStackLogging=1 leaks --atExit -- ./app args     # plus the allocation stack of each leak
```

Look for `Process N: X leaks for Y total leaked bytes`. With stack logging, each
leak shows where it was allocated. Scale the input up (run more iterations) and
see whether the count scales with it.

Other macOS tools: `heap <pid>` (live heap by class), `vmmap <pid>`,
`malloc_history <pid> <addr>` (with `MallocStackLogging=1`), Instruments' Leaks
and Allocations templates.

### General method

1. Reproduce reliably. Find the smallest input that still fails.
2. Read the *first* error, not the last.
3. State a hypothesis you can falsify, and pick the one tool that tests it.
4. Ask "who last wrote this value?": that's what watchpoints are for.
5. When the crash site looks innocent, the corruption happened earlier. Go upstream.
6. After fixing, ask: does this fix the root cause, or just this input?

---

## Phase 2: production incidents

`incidents/app/` contains a working "link page service" and a multi-threaded
request simulator. See `incidents/app/README.md`. Incidents (wrong data,
slow performance, memory growth, race condition, deadlock, compound failures)
will be added one at a time. For those, ThreadSanitizer (`-fsanitize=thread`,
can't be combined with ASan) and Instruments' Time Profiler / `sample <pid>`
join the toolbox.
