# INC-2102: Frame archiver aborts as soon as the first keyframe is archived

**Reporter:** Video pipeline on-call  **Severity:** P1

The archiver crashes immediately on every job:

```
$ ./frame-archiver 48
zsh: trace trap  ./frame-archiver 48
```

No output, no error message, exit code 133. One of the ingest boxes logged `malloc: *** error for object 0x...: pointer being freed was not allocated` right before the same crash.

If I hack the input so no frame is a keyframe, it runs to completion. The decode and stats code hasn't been touched in months. The `Archive` class was added last sprint.

**Done when:** `./test.sh` passes (sanitizer build plus a `leaks` check).
