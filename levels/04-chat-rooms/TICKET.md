# INC-2088: Message counts in room stats are wrong

**Reporter:** Community team  **Severity:** P3

We use the transcript replay tool to compute the "most active members" leaderboard each week. Some people's `sent=` counts are lower than what they actually posted. Example from this week's sample: ben posted 3 messages in #general (I counted by hand in the transcript) but the report shows `sent=2`. chen shows 1 in #dev but posted 2.

It isn't everyone and it isn't consistent between rooms. The total `(N messages)` at the top of each room looks right. It started after we shipped the "mentioning someone invites them to the room" feature.

Someone on the platform team said the sanitizer build "explodes" on this input but didn't have time to look.

**Done when:** `./test.sh` passes.
