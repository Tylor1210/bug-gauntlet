# INC-2057: Daily revenue summary shows negative millions on CI

**Reporter:** Finance automation (via #ci-alerts)  **Severity:** P2

The order-totals summary is used to reconcile against the payment processor every morning. Since we turned on the sanitizer build in CI, the summary job's numbers are nonsense:

```
1002   bob        pickup        1 -$10947902.76
1004   dmitri     pickup        4 -$10947887.36

5 orders (2 pickup), revenue -$21895628.40
```

When I run the regular debug build on my laptop with the same `orders.txt`, every number looks correct. Ops says the production box occasionally reports "a few cents to a few dollars off" on some days and they've been manually adjusting it. Nobody has been able to reproduce that one on purpose.

CI is not flaky here: same input, same wrong numbers, every run.

**Done when:** `./test.sh` passes (it uses the sanitizer build).
