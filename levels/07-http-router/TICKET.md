# INC-2133: Access log shows blank handler names

**Reporter:** SRE  **Severity:** P2

After the router refactor (the "candidate prefiltering" PR, which was supposed to be a pure performance change), the request log stopped showing which handler served most requests:

```
GET   /u/maya                    -> 
GET   /api/v1/health             -> 
GET   /favicon.ico               -> static.not_found_page
```

404s still log correctly. Everything else is blank, and the hit-count table at the bottom lumps them all under an empty name. On one staging box the log lines contained garbage bytes instead of blanks. Requests still seem to reach the right place in prod, as far as anyone can tell.

**Done when:** `./test.sh` passes.
