# Scoring checklist

Fill this in for every run. Keep the criteria the same for development and final evaluation runs.

**Run:** v2-TrackerEnemy-run1
**Model + version:** Sol 6.1 (per Git history)
**Date:** 2026-11-07 (run commit)
**Guide version:** v2
**Class / files in focus:** TrackerEnemy â€” TrackerEnemy.cpp, TrackerEnemy.h

| # | Criterion | Result | Notes |
|---|---|---|---|
| 1 | Compiles (full project build) | Y | Initial full editor target build succeeded (425 actions). Retry build was blocked by active Live Coding. |
| 2 | Behavior preserved (automation tests + own test cases pass) | Unverified | TrackerEnemy I/O equivalence was queued. Editor startup asserted in the AI Assistant tab before the test began. Retry blocked by active Live Coding. |
| 3 | Correct opportunity (vs. ground truth) | Unverified | No independently labeled ground truth located. The diff contains candidate extractions. |
| 4 | Complexity (Lizard CCN of refactored methods) | BindTrackerPerception 7 â†’ 4; HandleNoiseLead 9 â†’ 8 | Lizard 1.26.1. Original methods only. |
| 5 | Method length (Lizard NLOC) | BindTrackerPerception 26 â†’ 16; HandleNoiseLead 15 â†’ 12 | Lizard 1.26.1. Original methods only. |
| 6 | Unrequested changes (renames, reformatting, logic, macros, extra header edits) | N | No unrelated edits found. |
| 7 | Header changes limited to private helper declarations | Y | Only new private helper declarations. |
| 8 | Report accurate (matches `git diff`) | Y | Reported source edits match 7827cdd â†’ d03aac6. |

**Helpers added:** BindTrackerSuspicion, HasVisiblePlayer, ReplayCurrentlySeenActors
**Failure types seen (FT IDs, add FT13+ for new ones):** No FT ID assigned from source review.
**Other notes:** Git comparison: 7827cdd â†’ d03aac6. Unreal logs: C:\Users\aamir\Documents\WolfOfTheDesert\WolfOFTheDesertRefactored\Saved\RefactorRunScoring.
