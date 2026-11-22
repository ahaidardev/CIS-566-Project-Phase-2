# Scoring checklist

Fill this in for every run. Keep the criteria the same for development and final evaluation runs.

**Run:** v2-AliCharacter-run1
**Model + version:** Sol 6.1 (per Git history)
**Date:** 2026-11-07 (run commit)
**Guide version:** v2
**Class / files in focus:** AliCharacter - class source and header, unchanged

| # | Criterion | Result | Notes |
|---|---|---|---|
| 1 | Compiles (full project build) | N | Full build failed because current code expects GetMantleInputAction and Dialogue. The run made zero source edits, so these are snapshot compatibility failures. |
| 2 | Behavior preserved (automation tests + own test cases pass) | Unverified | Tests not run because the full project build failed. |
| 3 | Correct opportunity (vs. ground truth) | Unverified | No independently labeled ground truth located. Run left the source unchanged. |
| 4 | Complexity (Lizard CCN of refactored methods) | N/A | Lizard 1.26.1. Original methods only. |
| 5 | Method length (Lizard NLOC) | N/A | Lizard 1.26.1. Original methods only. |
| 6 | Unrequested changes (renames, reformatting, logic, macros, extra header edits) | N | No source diff. |
| 7 | Header changes limited to private helper declarations | Y | Header unchanged. |
| 8 | Report accurate (matches `git diff`) | Y | Git confirms unchanged source. |

**Helpers added:** None
**Failure types seen (FT IDs, add FT13+ for new ones):** No FT ID assigned from source review.
**Other notes:** Git comparison: ffe830c to ebbeb2f. Unreal logs: C:\Users\aamir\Documents\WolfOfTheDesert\WolfOFTheDesertRefactored\Saved\RefactorRunScoring.
