### ABaseEnemy.cpp
Status: Unchanged

Reason: No method bodies; this translation unit only contains includes.

Header changes: none
Considered but not extracted: None; there are no method bodies in this file (G7).
Behavior preservation: No helpers added here; all includes remain unchanged.
Compilation: Not built (build configuration and Unreal dependencies cannot be read within the guide's permitted five-file scope)
Pre-existing issues noticed (not changed): none

### 01_Damage.inl
Status: Unchanged

Reason: No fragment met §2 without extracting from short, single-purpose methods or creating a pass-through.

Header changes: none
Considered but not extracted: ApplyGrenadeBlast and TakeDamage are short, single-purpose methods (N5); TryInterceptIncomingDamage_Implementation has only a return statement (N5).
Behavior preservation: No helpers added here; all damage methods remain unchanged.
Compilation: Not built (build configuration and Unreal dependencies cannot be read within the guide's permitted five-file scope)
Pre-existing issues noticed (not changed): none

### 02_LifecycleAndMovementSettings.inl
Status: Refactored

| Method | Helper (signature) | Lines moved (from file as it was at start of this run) | What it does | Call sites |
|---|---|---|---|---|
| InitializeMovementSettings | void ConfigurePatrolMovementSpeeds() | 32–34 | Sets patrol walking speeds for every rotation mode and stance. | 1 (InitializeMovementSettings) |
| InitializeMovementSettings | void ConfigureInvestigationMovementSpeeds() | 35–42 | Sets investigation walking thresholds and running speeds for every rotation mode and stance. | 1 (InitializeMovementSettings) |
| InitializeMovementSettings | void ConfigureCombatMovementSpeeds() | 45–53 | Sets combat walking thresholds, running speeds, and minimum sprint speeds. | 1 (InitializeMovementSettings) |
| InitializeMovementSettings | void ConfigureCombatPositioningMovementSpeeds() | 56–67 | Disables direction-dependent positioning speeds and sets walking speeds and running/sprint minima. | 1 (InitializeMovementSettings) |

Header changes: Added private declarations void ConfigurePatrolMovementSpeeds(); void ConfigureInvestigationMovementSpeeds(); void ConfigureCombatMovementSpeeds(); void ConfigureCombatPositioningMovementSpeeds();
Considered but not extracted: AABaseEnemy constructor and BeginPlay are short, single-purpose initialization/orchestration methods (N5); BeginPlay's existing InitializeMovementSettings call cannot be wrapped (N3). The four loops only look similar: their targets, thresholds, and effects differ, so they are not shared under W6. Existing helpers were not restructured or wrapped (N6).
Behavior preservation: ConfigurePatrolMovementSpeeds retains its complete nested loop and chained assignment, called after the same two duplications under the original MovementSettings guard.
Behavior preservation: ConfigureInvestigationMovementSpeeds retains its complete nested loop, comment, 0.25f threshold, and assignment order, called immediately after patrol configuration.
Behavior preservation: ConfigureCombatMovementSpeeds retains all nested-loop statements and the FMath::Max call inside the same loop, called after the original combat duplication and before positioning/role duplication.
Behavior preservation: ConfigureCombatPositioningMovementSpeeds retains every assignment, literal, and per-stance FMath::Max call in order, called after both original positioning/role duplications under the same guard.
Compilation: Not built (build configuration and Unreal dependencies cannot be read within the guide's permitted five-file scope; static review and scoped brace/declaration/definition/call-count checks passed, but runtime equivalence was not verified)
Pre-existing issues noticed (not changed): none

### 03_LocomotionAndRotation.inl
Status: Refactored

| Method | Helper (signature) | Lines moved (from file as it was at start of this run) | What it does | Call sites |
|---|---|---|---|---|
| RefreshStealthLocomotion | UAlsMovementSettings* SelectStealthMovementSettings(const bool bCombat, const bool bInvestigating) | 34–39 | Selects combat, investigation, or patrol settings and applies the walking combat-positioning choice. | 1 (RefreshStealthLocomotion) |
| RefreshStealthLocomotion | void ApplyCombatRoleSpeedsAndInvalidateMovementCache(const float Speed, const FGameplayTag RoleGait) | 46–62 | Updates role gait thresholds and cached role values, then conditionally invalidates applied movement settings. | 1 (RefreshStealthLocomotion) |

Header changes: Added private declarations UAlsMovementSettings* SelectStealthMovementSettings(const bool bCombat, const bool bInvestigating); void ApplyCombatRoleSpeedsAndInvalidateMovementCache(const float Speed, const FGameplayTag RoleGait);
Considered but not extracted: Tracker/focus computations and the route-focus clearing block depend on auto-typed locals whose concrete declaration types cannot be copied from the permitted files (N4). EnemyMovementTask::Resolve updates both DesiredSettings and TreeDesiredGait, preventing a one-result extraction (H3, N2), and a wrapper would violate N3. Tick, SmoothEnemyRotation, grounded-rotation overrides, and SetupPlayerInputComponent are short or existing helpers/wrappers (N3, N5, N6).
Behavior preservation: SelectStealthMovementSettings retains the ternary operand order, comment, and bCombat && GetDesiredGait() condition; its one returned pointer initializes the original local at the same position, after all original guards and before role overrides.
Behavior preservation: ApplyCombatRoleSpeedsAndInvalidateMovementCache remains inside the original role-validity and changed-speed/gait guards; it retains complete nested loops, ternaries, cached assignments, comments, and conditional invalidation in their original order, before the role settings override and movement application.
Compilation: Not built (build configuration and Unreal dependencies cannot be read within the guide's permitted five-file scope; static review and scoped brace/declaration/definition/call-count checks passed, but runtime equivalence was not verified)
Pre-existing issues noticed (not changed): none

### ABaseEnemy.h
Status: Refactored

| Method | Helper (signature) | Lines moved (from file as it was at start of this run) | What it does | Call sites |
|---|---|---|---|---|
| InitializeMovementSettings | void ConfigurePatrolMovementSpeeds() | None; declaration added after original line 77 | Declares the private patrol-speed helper. | 1 (InitializeMovementSettings, 02_LifecycleAndMovementSettings.inl) |
| InitializeMovementSettings | void ConfigureInvestigationMovementSpeeds() | None; declaration added after original line 77 | Declares the private investigation-speed helper. | 1 (InitializeMovementSettings, 02_LifecycleAndMovementSettings.inl) |
| InitializeMovementSettings | void ConfigureCombatMovementSpeeds() | None; declaration added after original line 77 | Declares the private combat-speed helper. | 1 (InitializeMovementSettings, 02_LifecycleAndMovementSettings.inl) |
| InitializeMovementSettings | void ConfigureCombatPositioningMovementSpeeds() | None; declaration added after original line 77 | Declares the private combat-positioning-speed helper. | 1 (InitializeMovementSettings, 02_LifecycleAndMovementSettings.inl) |
| RefreshStealthLocomotion | UAlsMovementSettings* SelectStealthMovementSettings(const bool bCombat, const bool bInvestigating) | None; declaration added after original line 78 | Declares the private settings-selection helper. | 1 (RefreshStealthLocomotion, 03_LocomotionAndRotation.inl) |
| RefreshStealthLocomotion | void ApplyCombatRoleSpeedsAndInvalidateMovementCache(const float Speed, const FGameplayTag RoleGait) | None; declaration added after original line 78 | Declares the private role-speed/cache-update helper. | 1 (RefreshStealthLocomotion, 03_LocomotionAndRotation.inl) |

Header changes: Added only the six private declarations listed above in the existing private section; no macros, existing declarations, members, comments, or access sections changed.
Considered but not extracted: No method bodies in the header; G2 permits only adding private helper declarations.
Behavior preservation: ConfigurePatrolMovementSpeeds is declared private, non-const, and without UFUNCTION; no existing declaration changed.
Behavior preservation: ConfigureInvestigationMovementSpeeds is declared private, non-const, and without UFUNCTION; no existing declaration changed.
Behavior preservation: ConfigureCombatMovementSpeeds is declared private, non-const, and without UFUNCTION; no existing declaration changed.
Behavior preservation: ConfigureCombatPositioningMovementSpeeds is declared private, non-const, and without UFUNCTION; no existing declaration changed.
Behavior preservation: SelectStealthMovementSettings is declared private, non-const, and without UFUNCTION, using the original const bool local types and one UAlsMovementSettings* result.
Behavior preservation: ApplyCombatRoleSpeedsAndInvalidateMovementCache is declared private, non-const, and without UFUNCTION, using the original const float and const FGameplayTag local types, with no out-parameters.
Compilation: Not built (build configuration and Unreal dependencies cannot be read within the guide's permitted five-file scope; scoped file diagnostics returned no entries, which is not proof of compilation)
Pre-existing issues noticed (not changed): none
