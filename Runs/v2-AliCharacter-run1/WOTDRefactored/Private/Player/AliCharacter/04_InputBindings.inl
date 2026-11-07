// AliCharacter: InputBindings. Included once by AliCharacter.cpp; retains original definition order.

void AAliCharacter::SetupPlayerInputComponent(UInputComponent* Input)
{
	Super::SetupPlayerInputComponent(Input);

	auto* EnhancedInput{Cast<UEnhancedInputComponent>(Input)};
	if (IsValid(EnhancedInput))
	{
		EnhancedInput->BindAction(LookMouseAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnLookMouse);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnLook);
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnMove);
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnSprint);
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &ThisClass::Input_OnSprintEnd);
		EnhancedInput->BindAction(WalkAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnWalk);
		EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnCrouch);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnJump);
		EnhancedInput->BindAction(AimAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnAim);
		// Release and cancellation must clear the held aim state, including with a selected card.
		EnhancedInput->BindAction(AimAction, ETriggerEvent::Completed, this, &ThisClass::Input_OnAim);
		EnhancedInput->BindAction(AimAction, ETriggerEvent::Canceled, this, &ThisClass::Input_OnAim);
		EnhancedInput->BindAction(RagdollAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnRagdoll);
		EnhancedInput->BindAction(RollAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnRoll);
		EnhancedInput->BindAction(RotationModeAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnRotationMode);
		EnhancedInput->BindAction(SwitchShoulderAction, ETriggerEvent::Triggered, this,
								  &ThisClass::Input_OnSwitchShoulder);
		EnhancedInput->BindAction(CardSelectLeft, ETriggerEvent::Triggered, this, &ThisClass::Input_OnCardSelectLeft);
		EnhancedInput->BindAction(CardSelectRight, ETriggerEvent::Triggered, this, &ThisClass::Input_OnCardSelectRight);

		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnCoverMove);
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Completed, this, &ThisClass::Input_OnCoverMoveEnd);
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Canceled, this, &ThisClass::Input_OnCoverMoveEnd);
		EnhancedInput->BindAction(ParkourAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnParkour);
		EnhancedInput->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnShoot);
	}
}
