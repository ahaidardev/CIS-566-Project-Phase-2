### 11_PowerWeaponAndViewMode.inl
Status: Unchanged
Reason: No fragment met §2 — the only active method bodies are `PowerWeaponShot` (lines 372–384, ~7 lines, single purpose) and `Input_OnViewMode` (lines 509–514, one statement), both blocked by N5; every other method in the file is commented out.

Considered but not extracted:
- `PowerWeaponShot` (lines 372–384): already short and does one thing (performs the weapon aim trace); the rest of its body is commented out — blocked by N5.
- `Input_OnViewMode` (lines 509–514): a single ternary `SetViewMode` call — blocked by N5.
- All other methods in the file (`CheckObsThicknessEvent`, `CheckObsHeightEvent`, `CheckObsHeight`, `CheckObsInfo`, `CheckObsGap`, `CheckObsGapPure`, `CheckThickness`, `GetPlayerSave`, `UsePlayerSave`, `IsMantlingAllowedToStartBP`, `ThrowShockChip`, `AutoSelect`, `ThrowGrenade`, `HairLODSetup`, `DiscardSelectedCard`, `ThrowSmoke`, `TraceReq`, `EnableElectroShield`, `DisableElectricShield`, `EquipCard`, `AbsorbShard`, `EnableInvisibility`, grapple stubs, `HideMeshFx`, `DisableInvisibility`, `UpdateThrowableTrajectory`, `PerformLineTrace`, `HandleCoverMovement`, `UpdateVaultingState`, `OnMantlingStarted_Implementation`): entirely commented out, so they contain no method bodies to refactor (G5 forbids rewriting comments).

Header changes: none
Behavior preservation: No code was changed, so behavior is identical by construction.
Compilation: Not built (no changes were made; the project's full Unreal Engine source tree is not available in this folder)
Pre-existing issues noticed (not changed): none in the active code; note the trailing commented-out fragment at lines 548–553 (a duplicated, unterminated commented `OnMantlingStarted_Implementation` block) is inert comment text, confirmed present before this run.
