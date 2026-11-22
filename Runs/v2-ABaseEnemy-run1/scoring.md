# Scoring checklist

Fill this in for every run. Keep the criteria the same for development and final evaluation runs.

**Run:** v2-ABaseEnemy-run1
**Model + version:** Sol 6.1 (per Git history)
**Date:** 2026-11-07 (run commit)
**Guide version:** v2
**Class / files in focus:** ABaseEnemy â€” 02_LifecycleAndMovementSettings.inl, 03_LocomotionAndRotation.inl, ABaseEnemy.h

| # | Criterion | Result | Notes |
|---|---|---|---|
| 1 | Compiles (full project build) | Blocked | Full editor build refused while Live Coding was active. See ABaseEnemy-build.log. |
| 2 | Behavior preserved (automation tests + own test cases pass) | Unverified | Class test could not run because the full build was blocked. |
| 3 | Correct opportunity (vs. ground truth) | Unverified | No independently labeled ground truth located. The diff contains candidate extractions. |
| 4 | Complexity (Lizard CCN of refactored methods) | InitializeMovementSettings 10 â†’ 2; RefreshStealthLocomotion 64 â†’ 55 | Lizard 1.26.1. Original methods only. |
| 5 | Method length (Lizard NLOC) | InitializeMovementSettings 41 â†’ 15; RefreshStealthLocomotion 87 â†’ 69 | Lizard 1.26.1. Original methods only. |
| 6 | Unrequested changes (renames, reformatting, logic, macros, extra header edits) | Y | Existing comments move into helpers, contrary to G5. A return is also under-indented. |
| 7 | Header changes limited to private helper declarations | Y | Only new private helper declarations. |
| 8 | Report accurate (matches `git diff`) | Y | Reported source edits match e51b566 â†’ 57d1291. |

**Helpers added:** ApplyCombatRoleSpeedsAndInvalidateMovementCache, ConfigureCombatMovementSpeeds, ConfigureCombatPositioningMovementSpeeds, ConfigureInvestigationMovementSpeeds, ConfigurePatrolMovementSpeeds, SelectStealthMovementSettings
**Failure types seen (FT IDs, add FT13+ for new ones):** FT8 (comment relocation, per guide traceability).
**Other notes:** Git comparison: e51b566 â†’ 57d1291. Unreal logs: C:\Users\aamir\Documents\WolfOfTheDesert\WolfOFTheDesertRefactored\Saved\RefactorRunScoring.
