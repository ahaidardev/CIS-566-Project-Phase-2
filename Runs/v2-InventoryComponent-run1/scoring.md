# Scoring checklist

Fill this in for every run. Keep the criteria the same for development and final evaluation runs.

**Run:** v2-InventoryComponent-run1
**Model + version:** Sol 6.1 (per Git history)
**Date:** 2026-11-07 (run commit)
**Guide version:** v2
**Class / files in focus:** InventoryComponent - 02_CardMutations.inl, 03_RefillsAndReserve.inl, 04_WheelPresentationAndCleanup.inl, InventoryComponent.h

| # | Criterion | Result | Notes |
|---|---|---|---|
| 1 | Compiles (full project build) | Y | Full editor target build exit 0. See build log in the evidence directory. |
| 2 | Behavior preserved (automation tests + own test cases pass) | Y | WOTD.Refactoring.IOEquivalence.InventoryComponent, process exit 0. Passed. |
| 3 | Correct opportunity (vs. ground truth) | Unverified | No independently labeled ground truth located. The diff contains candidate extractions. |
| 4 | Complexity (Lizard CCN of refactored methods) | AddCardToInventory_Implementation 8 to 2; EndPlay 3 to 1; RemoveSelectedCard 13 to 9; SwapSelectedCardWithReserve 6 to 6; UpdateAutoRefills 12 to 7; UpdateCardWheel 45 to 12 | Lizard 1.26.1. Original methods only. |
| 5 | Method length (Lizard NLOC) | AddCardToInventory_Implementation 38 to 12; EndPlay 17 to 6; RemoveSelectedCard 41 to 24; SwapSelectedCardWithReserve 22 to 17; UpdateAutoRefills 26 to 17; UpdateCardWheel 106 to 47 | Lizard 1.26.1. Original methods only. |
| 6 | Unrequested changes (renames, reformatting, logic, macros, extra header edits) | Y | Existing comments move into helpers, contrary to G5. |
| 7 | Header changes limited to private helper declarations | Y | Only new private helper declarations. |
| 8 | Report accurate (matches `git diff`) | Y | Reported source edits match 474f19a to f797c67. |

**Helpers added:** AcquireCardWheel, ConsumePendingRefillCharge, EquipReserveStack, GrantDueRefillCharges, MoveEquippedStackToReserve, PlaceCardInWheelOrReserve, RemoveSlotAndClampSelection, ScheduleAutoRefill, TearDownCardWidgets, UpdateHandLayoutToggle, UpdateInventoryPanel, UpdateWheelInputAndAnimation
**Failure types seen (FT IDs, add FT13+ for new ones):** FT8 (comment relocation, per guide traceability).
**Other notes:** Git comparison: 474f19a to f797c67. Unreal logs: C:\Users\aamir\Documents\WolfOfTheDesert\WolfOFTheDesertRefactored\Saved\RefactorRunScoring.
