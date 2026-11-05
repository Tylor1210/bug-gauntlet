# linkpage-sim: the link page service

A small in-memory "link in bio" service plus a load generator. It is the
codebase for Phase 2: production-style incidents will be injected into it one
at a time. Right now it is the **known-good baseline**.

## What it does

- **`LinkStore`** (`store.h/.cpp`): profiles (`handle`, display name, bio) each
  with an ordered list of links (title, URL, click count). Create profiles,
  update bios, add/remove links (max 50 per profile), record clicks, render a
  public page as text. One mutex guards the whole store. Removing a link moves
  its clicks into an `archived_clicks` total so no clicks are lost.
- **`Analytics`** (`analytics.h/.cpp`): per-profile view and click counters and a
  "trending" top-N by views. Has its own mutex.
- **`Simulator`** (`simulator.h/.cpp`): seeds N profiles, then runs worker
  threads that each issue a weighted random mix of requests:
  60% view page, 25% click, 8% add link, 4% remove link, 2% create profile,
  1% update bio. Every op is timed.
- **`log`** (`log.h/.cpp`): thread-safe leveled logger to stderr
  (`--verbose` enables DEBUG).
- **`main.cpp`**: prints per-op counts and p50/p99 latency, store totals, the
  trending list, a state checksum, and checks invariants:
  - store clicks (live + archived) == successful click ops
  - analytics clicks == successful click ops
  - analytics views == successful page views

  Exit code 0 when all invariants hold, 2 when any fail.

## Running

```bash
cmake -S . -B build && cmake --build build      # from the repo root
build/bin/linkpage-sim --ops 20000 --threads 1 --seed 42
build/bin/linkpage-sim --ops 200000 --threads 8
```

Flags: `--ops N` (total across threads, default 20000), `--threads T`
(default 1), `--seed S` (default 42), `--profiles P` initial profiles
(default 200), `--verbose`.

With `--threads 1` the run is deterministic for a given seed: counts, trending
and the state checksum are identical every run (latencies vary). Baseline:

```
--ops 20000 --threads 1 --seed 42  ->  state checksum: d3dc1a302b0db1b0
```

The baseline is clean under AddressSanitizer, UndefinedBehaviorSanitizer and
ThreadSanitizer (`-fsanitize=thread`) at 8 threads.

## Incidents

Planned, one at a time: wrong data, slow performance, memory growth, race
condition, deadlock, and multiple interacting bugs with misleading logs. Each
will arrive as its own ticket in `incidents/`.
