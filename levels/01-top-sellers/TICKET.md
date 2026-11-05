# INC-2041: Top sellers report crashes with no output

**Reporter:** Marta (Merch Ops)  **Severity:** P3

The nightly "top sellers" job that we pipe into the merch Slack channel stopped posting this week. When I run it by hand against yesterday's export it just dies:

```
$ ./top-sellers sales.csv
zsh: segmentation fault  ./top-sellers sales.csv
```

No output at all, not even the header line. Last month's export (fewer products, we only had 4 SKUs back then) still runs fine. Nothing about the CSV format changed, it's the same export from the same tool.

Attached: `sales.csv` (yesterday's export). `expected_output.txt` is what the report should say, I put it together by hand from the spreadsheet.

**Done when:** `./test.sh` passes.
