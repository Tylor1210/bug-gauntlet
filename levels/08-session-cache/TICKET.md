# INC-2150: Active users randomly logged out; auth service crashed twice overnight

**Reporter:** Support escalation + on-call  **Severity:** P1

Support has a pile of tickets from users who were *actively clicking around* and got a "session expired" 401. Meanwhile users who have been idle for a long time stay logged in. The session cache is supposed to evict the least recently active session when it's full (capacity is 4 in the replay tool, 50k in prod).

From the replay of last night's sample (`events.txt`):

```
t=151  401 for eli (session expired)
```

eli logged in at t=130. Several other users were idle longer than that and weren't evicted.

Separately, the auth service segfaulted twice overnight with a stack in `SessionCache`. Could be related, could be the new hardware.

**Done when:** `./test.sh` passes (sanitizer build plus a `leaks` check).
