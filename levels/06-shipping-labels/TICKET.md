# INC-2119: Label printer job fails in CI on international contact list

**Reporter:** Fulfillment engineering  **Severity:** P2

The label formatter runs fine on the warehouse machine: it prints every label and exits 0. Labels for very long addresses get cut off, which is expected (the label stock is small and the formatter is supposed to truncate).

But the CI job that runs it under AddressSanitizer fails every time on `contacts.txt`, and last week the warehouse box printed a label with what looked like part of the *next* customer's postal code mashed into it. We couldn't reproduce that one.

Short US-only contact lists pass in CI.

**Done when:** `./test.sh` passes. Truncation of long labels is still expected; see `expected_output.txt`.
