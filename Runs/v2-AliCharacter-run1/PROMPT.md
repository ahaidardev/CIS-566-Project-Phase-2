Follow refactor-guide-v2.md in this folder exactly.

Target class: AliCharacter. Its files are:
- WOTDRefactored/Private/Player/AliCharacter.cpp
- WOTDRefactored/Private/Player/AliCharacter/01_VisibilityAndConstruction.inl
- WOTDRefactored/Private/Player/AliCharacter/02_Invisibility.inl
- WOTDRefactored/Private/Player/AliCharacter/03_LifecycleAndCamera.inl
- WOTDRefactored/Private/Player/AliCharacter/04_InputBindings.inl
- WOTDRefactored/Private/Player/AliCharacter/05_LocomotionInput.inl
- WOTDRefactored/Private/Player/AliCharacter/06_CoverAndWeaponInput.inl
- WOTDRefactored/Private/Player/AliCharacter/07_Initialization.inl
- WOTDRefactored/Private/Player/AliCharacter/08_WeaponTracing.inl
- WOTDRefactored/Private/Player/AliCharacter/09_InteractionTracing.inl
- WOTDRefactored/Private/Player/AliCharacter/10_CameraAndObstacleChecks.inl
- WOTDRefactored/Private/Player/AliCharacter/11_PowerWeaponAndViewMode.inl
- WOTDRefactored/Public/Player/AliCharacter.h

Only refactor methods in these files: 11_PowerWeaponAndViewMode.inl. Leave all other files unchanged.

Apply Extract Method refactoring to this class as the guide allows. Do not read or edit anything outside this folder.

When finished, write your report, in the format from section 7 of the guide, to report.md in this folder.
