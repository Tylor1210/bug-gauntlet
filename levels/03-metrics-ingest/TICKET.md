# PAGE: metrics-ingest RSS growth, OOM-killed every ~9 hours

**Source:** PagerDuty, `metrics-ingest-worker`  **Severity:** P2

```
[03:12:44] ALERT metrics-ingest-worker-2 container memory 94% of limit (3.76 GiB / 4 GiB)
[03:14:02] metrics-ingest-worker-2 OOMKilled, restart count 7
```

Memory graph is a straight diagonal line from each restart to the OOM kill. Throughput and output are correct right up until it dies; dashboards show the right aggregates. The worker runs the same parse/aggregate loop over each batch file forever.

Interesting: worker-4 handles the `prod-only` feed and has been up for 11 days with flat memory. Workers 1–3 handle the mixed feed (prod + synthetic probes + some late-arriving data).

Repro locally by replaying the sample batch many times: `./metrics-ingest metrics.log 200`.

**Done when:** `./test.sh` passes (it checks output and runs `leaks`).
