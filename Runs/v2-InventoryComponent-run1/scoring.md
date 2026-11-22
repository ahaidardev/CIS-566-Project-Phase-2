# Scoring checklist

Fill this in for every run. Keep the criteria the same for development and final evaluation runs.

**Run:** v2-InventoryComponent-run1
**Model + version:** Sol 6.1 (per Git history)
**Date:** 2026-11-07 (run commit)
**Guide version:** v2
**Class / files in focus:** InventoryComponent â€” 02_CardMutations.inl, 03_RefillsAndReserve.inl, 04_WheelPresentationAndCleanup.inl, InventoryComponent.h

| # | Criterion | Result | Notes |
|---|---|---|---|
| 1 | Compiles (full project build) | Blocked | Full editor build refused while Live Coding was active. See InventoryComponent-build.log. |
| 2 | Behavior preserved (automation tests + own test cases pass) | Unverified | Class test could not run because the full build was blocked. |
| 3 | Correct opportunity (vs. ground truth) | Unverified | No independently labeled ground truth located. The diff contains candidate extractions. |
| 4 | Complexity (Lizard CCN of refactored methods) | AddCardToInventory_Implementation 8 â†’ 2; EndPlay 3 â†’ 1; RemoveSelectedCard 13 â†’ 9; SwapSelectedCardWithReserve 6 â†’ 6; UpdateAutoRefills 12 â†’ 7; UpdateCardWheel 45 â†’ 12 | Lizard 1.26.1. Original methods only. |
| 5 | Method length (Lizard NLOC) | AddCardToInventory_Implementation 38 â†’ 12; EndPlay 17 â†’ 6; RemoveSelectedCard 41 â†’ 24; SwapSelectedCardWithReserve 22 â†’ 17; UpdateAutoRefills 26 â†’ 17; UpdateCardWheel 106 â†’ 47 | Lizard 1.26.1. Original methods only. |
| 6 | Unrequested changes (renames, reformatting, logic, macros, extra header edits) | Y | Existing comments move into helpers, contrary to G5. |
| 7 | Header changes limited to private helper declarations | Y | Only new private helper declarations. |
| 8 | Report accurate (matches `git diff`) | Y | Reported source edits match 474f19a â†’ f797c67. |

**Helpers added:** AcquireCardWheel, ConsumePendingRefillCharge, EquipReserveStack, GrantDueRefillCharges, MoveEquippedStackToReserve, PlaceCardInWheelOrReserve, RemoveSlotAndClampSelection, ScheduleAutoRefill, TearDownCardWidgets, UpdateHandLayoutToggle, UpdateInventoryPanel, UpdateWheelInputAndAnimation
**Failure types seen (FT IDs, add FT13+ for new ones):** FT8 (comment relocation, per guide traceability).
**Other notes:** Git comparison: 474f19a â†’ f797c67. Unreal logs: C:\Users\aamir\Documents\WolfOfTheDesert\WolfOFTheDesertRefactored\Saved\RefactorRunScoring.
