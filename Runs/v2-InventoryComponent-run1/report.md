# Extract Method Report — InventoryComponent (UInventory)

### InventoryComponent.cpp
Status: Unchanged

Reason: The file contains only `#include` directives and no method bodies, so there is nothing to extract (G7).

### 01_LifecycleAndSelection.inl
Status: Unchanged

Reason: Every method is either short and single-purpose (constructor, `BeginPlay`, `TickComponent`, getters, `SelectCard` — N5), or its only coherent fragments are blocked: in `CanAddCard` the card-counting logic is interrupted by an early `return true` at line 102 (W3), and the fragment before it assigns two locals (`TotalCards`, `ReservedClasses`) that are both used afterwards (H3); in `RefreshCardOverlay` the remaining fragments are single statements or a near pass-through to `ProcessEvent` (W1, N3).

### 02_CardMutations.inl
Status: Refactored

| Method | Helper (signature) | Lines moved (from file as it was at start of this run) | What it does | Call sites |
|---|---|---|---|---|
| AddCardToInventory_Implementation | `void ConsumePendingRefillCharge(const FCardItem& Card)` | 11–15 | Decrements the pending auto-refill charge for the picked-up card's class and removes the pending entry once no charges are missing | 1 (AddCardToInventory_Implementation) |
| AddCardToInventory_Implementation | `void PlaceCardInWheelOrReserve(const FCardItem& Card)` | 17–43 | Records the added card: increments an existing wheel stack or reserve stack, starts a new reserve stack when the wheel is full, or adds a new wheel slot (selecting it when it is the first card) | 1 (AddCardToInventory_Implementation) |
| RemoveSelectedCard | `void ScheduleAutoRefill(const TSubclassOf<UGSCGameplayAbility>& SelectedClass, const FCardItem& RemovedCard)` | 84–91 | Adds one missing charge to the pending-refill entry for the removed card, initializing its card data and countdown when the entry is new | 1 (RemoveSelectedCard) |
| RemoveSelectedCard | `void RemoveSlotAndClampSelection(const TSubclassOf<UGSCGameplayAbility>& SelectedClass)` | 97–109 | Removes the emptied card slot from the copy-count/data maps and slot order, then clamps `CurrentSlotIndex` back into range | 1 (RemoveSelectedCard) |

Header changes: added private declarations `void ConsumePendingRefillCharge(const FCardItem& Card);`, `void PlaceCardInWheelOrReserve(const FCardItem& Card);`, `void ScheduleAutoRefill(const TSubclassOf<UGSCGameplayAbility>& SelectedClass, const FCardItem& RemovedCard);`, `void RemoveSlotAndClampSelection(const TSubclassOf<UGSCGameplayAbility>& SelectedClass);`
Considered but not extracted: the guard `if (!CanAddCard(Card)) return {};` in AddCardToInventory_Implementation — already a single predicate call with the `return` at the call site (W1/N3); the validity guards at the top of RemoveSelectedCard — each contains a `return {}` that must stay at the call site and extracting the conditions alone would be pass-throughs (W3, N3).
Behavior preservation: ConsumePendingRefillCharge — the whole `if` statement (including its init-declaration of `Pending`) moved verbatim and is still called at the same point, so the decrement/remove happen exactly as before. PlaceCardInWheelOrReserve — the entire `if/else if/else` chain moved verbatim with branch order, conditions, and the first-card auto-select unchanged; it still runs after the pending-charge consumption and before the broadcasts. ScheduleAutoRefill — the outer condition `RefillType == Automatic && !bIsRecycling` stayed at the call site; the moved body is unchanged, so FindOrAdd/initialize/increment occur identically. RemoveSlotAndClampSelection — the outer condition `CardCopyCounts[SelectedClass] <= 0` stayed at the call site; the moved removals and index adjustment are verbatim and run in the same order before the broadcasts.
Compilation: Not built (this run folder is an isolated source excerpt with no .uproject/Build.cs/engine available, and the guide forbids using files outside it; all edits were re-read line by line instead)
Pre-existing issues noticed (not changed): none

### 03_RefillsAndReserve.inl
Status: Refactored

| Method | Helper (signature) | Lines moved (from file as it was at start of this run) | What it does | Call sites |
|---|---|---|---|---|
| UpdateAutoRefills | `FPendingCardRefill* GrantDueRefillCharges(FPendingCardRefill* Pending, const TSubclassOf<UGSCGameplayAbility>& Key)` | 21–30 | While the refill timer has elapsed and charges are missing, re-adds the card (if capacity allows), advances the timer, and refreshes the pending pointer, which it returns | 1 (UpdateAutoRefills) |
| SwapSelectedCardWithReserve | `void MoveEquippedStackToReserve(const TSubclassOf<UGSCGameplayAbility>& EquippedClass)` | 83–86 | Moves the equipped stack's count and card data into the reserve maps and removes them from the wheel maps | 1 (SwapSelectedCardWithReserve) |
| SwapSelectedCardWithReserve | `void EquipReserveStack(const TSubclassOf<UGSCGameplayAbility>& ReserveClass, int32 ReserveCount, const FCardItem& ReserveData)` | 87–89 | Installs the reserve stack's count and card data into the wheel maps and places its class in the currently selected slot | 1 (SwapSelectedCardWithReserve) |

Header changes: added private declarations `FPendingCardRefill* GrantDueRefillCharges(FPendingCardRefill* Pending, const TSubclassOf<UGSCGameplayAbility>& Key);`, `void MoveEquippedStackToReserve(const TSubclassOf<UGSCGameplayAbility>& EquippedClass);`, `void EquipReserveStack(const TSubclassOf<UGSCGameplayAbility>& ReserveClass, int32 ReserveCount, const FCardItem& ReserveData);`
Considered but not extracted: UpdateAutoRefills' full per-key loop body — contains `continue` targeting the enclosing `for` (W3); GetWheelCards' pending-card append — would need `Cards` as an out-parameter (N2) and the fragment contains a `Sort` lambda (N1); GetWheelCards' first collection loop — trivially readable, a name would not replace a comment (W1); GetReserveCards — single-purpose method with a lambda (W1, N1); the first-half reads/removes of the swap (lines 79–82) — assigns two locals (`ReserveCount`, `ReserveData`) used afterwards (H3).
Behavior preservation: GrantDueRefillCharges — the `while` loop (whose `break` targets the loop itself) moved verbatim; the refreshed `Pending` pointer is returned and reassigned at the call site, so the subsequent `MissingCharges == 0` check sees exactly the same pointer as before. MoveEquippedStackToReserve / EquipReserveStack — the nine mutation statements kept their exact original order split across two calls made in the original sequence, between the reserve-entry removals and the `OnCardSelected` broadcast.
Compilation: Not built (this run folder is an isolated source excerpt with no .uproject/Build.cs/engine available, and the guide forbids using files outside it; all edits were re-read line by line instead)
Pre-existing issues noticed (not changed): none

### 04_WheelPresentationAndCleanup.inl
Status: Refactored

| Method | Helper (signature) | Lines moved (from file as it was at start of this run) | What it does | Call sites |
|---|---|---|---|---|
| UpdateCardWheel, EndPlay | `void TearDownCardWidgets()` | 9–20 (UpdateCardWheel) and 115–126 (EndPlay), identical duplication (W6) | Closes the card wheel and inventory panel without animation, removes both from their parents, and nulls the widget pointers | 2 (UpdateCardWheel, EndPlay) |
| UpdateCardWheel | `void AcquireCardWheel(APlayerController* Controller)` | 50–69 | Finds the controller's HUD-embedded card wheel, or creates a hit-test-invisible one and adds it to the player screen | 1 (UpdateCardWheel) |
| UpdateCardWheel | `void UpdateInventoryPanel(APlayerController* Controller, bool bInventoryKeyDown)` | 71–83 | Binds this inventory to the panel, opens it on the Tab rising edge (when input is allowed), pumps its input, and closes it on key release | 1 (UpdateCardWheel) |
| UpdateCardWheel | `void UpdateHandLayoutToggle(APlayerController* Controller)` | 86–95 | Reads the hand-toggle keys and flips the wheel's hand layout on a rising edge when not recycling, paused, or browsing the inventory, then latches the key state | 1 (UpdateCardWheel) |
| UpdateCardWheel | `void UpdateWheelInputAndAnimation(APlayerController* Controller, bool bWheelHeld)` | 96–107 | Opens the wheel on the shoulder-button rising edge (when input is allowed), advances its animation, pumps its input, and closes it on release | 1 (UpdateCardWheel) |

Header changes: added private declarations `void TearDownCardWidgets();`, `void AcquireCardWheel(class APlayerController* Controller);`, `void UpdateInventoryPanel(class APlayerController* Controller, bool bInventoryKeyDown);`, `void UpdateHandLayoutToggle(class APlayerController* Controller);`, `void UpdateWheelInputAndAnimation(class APlayerController* Controller, bool bWheelHeld);`
Considered but not extracted: the owning-player-mismatch cleanup blocks (lines 26–39) — only similar to TearDownCardWidgets (single widget plus a flag reset each), not identical, so the helper may not be used there (W6); the panel-creation block (lines 40–45) — already short and self-explanatory (W1).
Behavior preservation: TearDownCardWidgets — both duplicated blocks were byte-identical; at each site the helper call replaces them in place, with the flag resets/`return` (UpdateCardWheel) and `PendingRefills.Empty()`/`Super::EndPlay` (EndPlay) still following unchanged. AcquireCardWheel — called inside the original `if (!CardWheel)` guard; the find-then-create sequence, its `break` (internal to the moved `for`), and widget setup order are verbatim. UpdateInventoryPanel — the three consecutive panel statements moved as a unit at the same position, so SetInventory/open/UpdateInput/close still run in the original order relative to wheel handling. UpdateHandLayoutToggle — `bHandToggleDown` is computed inside the helper exactly as before (same `||` order) and `bWasHandToggleKeyDown` is latched at the same point in the frame. UpdateWheelInputAndAnimation — open-on-rising-edge, animation advance, input pump, and close-on-release moved verbatim; `bWasWheelHeld`/`bWasInventoryKeyDown`/`bWasReserveWheelHeld` assignments remain at the call site after the helper, as in the original.
Compilation: Not built (this run folder is an isolated source excerpt with no .uproject/Build.cs/engine available, and the guide forbids using files outside it; all edits were re-read line by line instead)
Pre-existing issues noticed (not changed): `bWasReserveWheelHeld` is assigned from `bInventoryKeyDown` at original line 110 but never read anywhere in the class (confirmed present before this run)

### InventoryComponent.h
Status: Refactored (declarations only)

Header changes: twelve private helper declarations listed above were appended to the existing `private:` section; nothing else in the header was touched.
