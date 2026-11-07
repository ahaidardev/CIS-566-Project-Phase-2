// AliCharacter: LocomotionInput. Included once by AliCharacter.cpp; retains original definition order.

void AAliCharacter::Input_OnCardSelectLeft(const FInputActionValue& ActionValue)
{
	if (InventoryComponent->bIsRecycling)
		return;
	InventoryComponent->SelectCard(InventoryComponent->GetNextCard());
}

void AAliCharacter::Input_OnCardSelectRight(const FInputActionValue& ActionValue)
{
	if (InventoryComponent->bIsRecycling)
		return;
	InventoryComponent->SelectCard(InventoryComponent->GetPreviousCard());
}

void AAliCharacter::Input_OnLookMouse(const FInputActionValue& ActionValue)
{
	const auto Value{ActionValue.Get<FVector2D>()};

	AddControllerPitchInput(Value.Y * LookUpMouseSensitivity);
	AddControllerYawInput(Value.X * LookRightMouseSensitivity);

	// Re-clamp in the same frame as the input. The cover component also clamps every tick (the
	// anchor drifts as the wall curves), but doing it here as well means the player never gets a
	// frame of unclamped view out of a fast mouse flick.
	if (IsValid(CoverComponent))
	{
		CoverComponent->ClampLookToCover();
	}
}

void AAliCharacter::Input_OnLook(const FInputActionValue& ActionValue)
{
	const auto Value{ActionValue.Get<FVector2D>()};

	AddControllerPitchInput(Value.Y * LookUpRate);
	AddControllerYawInput(Value.X * LookRightRate);

	if (IsValid(CoverComponent))
	{
		CoverComponent->ClampLookToCover();
	}
}

void AAliCharacter::Input_OnMove(const FInputActionValue& ActionValue)
{
	//FProperty* Property =  GetClass()->FindPropertyByName(FName("WallDetected?"));
	//if(FBoolProperty* boolProperty = CastField<FBoolProperty>(Property)){
	//bool bWallDetected = boolProperty->GetPropertyValue_InContainer(this);

	if (IsValid(CoverComponent) && CoverComponent->GetCoverBool())
		return;

	const auto Value{UAlsVector::ClampMagnitude012D(ActionValue.Get<FVector2D>())};

	//if(!bWallDetected){
	const auto ForwardDirection{UAlsVector::AngleToDirectionXY(UE_REAL_TO_FLOAT(GetViewState().Rotation.Yaw))};
	const auto RightDirection{UAlsVector::PerpendicularCounterClockwiseXY(ForwardDirection)};

	AddMovementInput(ForwardDirection * Value.Y + RightDirection * Value.X);

	//}
	// if(bWallDetected){
	// 	//const auto ForwardDirection{UAlsMath::AngleToDirectionXY(UE_REAL_TO_FLOAT(GetViewState().Rotation.Yaw))};
	// 	const auto RightDirection{GetActorRightVector()};
	//
	// 	AddMovementInput(RightDirection * Value.X);
	//
	//
	// }
}
//}

void AAliCharacter::Input_OnSprint(const FInputActionValue& ActionValue)
{
	if (IsValid(CoverComponent) && CoverComponent->GetCoverBool())
		return;
	if (!ActionValue.Get<bool>())
	{
		Input_OnSprintEnd(ActionValue);
		return;
	}
	if (GetLocomotionAction().IsValid())
		return;
	if (!bSprintInputActive)
	{
		StanceBeforeSprint = GetDesiredStance();
		bSprintInputActive = true;
	}
	SetDesiredStance(AlsStanceTags::Standing);
	// Release aim before requesting sprint so ALS can enter the running locomotion.
	SetDesiredAiming(false);
	SetDesiredGait(AlsGaitTags::Sprinting);
}

void AAliCharacter::Input_OnSprintEnd(const FInputActionValue& InputActionValue)
{
	if (bSprintInputActive)
	{
		bSprintInputActive = false;
		SetDesiredGait(AlsGaitTags::Running);
		SetDesiredStance(StanceBeforeSprint);
	}
}

void AAliCharacter::Input_OnWalk()
{
	if (GetDesiredGait() == AlsGaitTags::Walking)
	{
		SetDesiredGait(AlsGaitTags::Running);
	}
	else if (GetDesiredGait() == AlsGaitTags::Running)
	{
		SetDesiredGait(AlsGaitTags::Walking);
	}
}

void AAliCharacter::Input_OnCrouch()
{

	if (IsValid(CoverComponent) && CoverComponent->GetCoverBool())
	{
		if (GetDesiredStance() == AlsStanceTags::Crouching)
		{
			if (!CoverComponent->CanStandInCover())
				return;
			else
				SetDesiredStance(AlsStanceTags::Standing);
		}
		else if (GetDesiredStance() == AlsStanceTags::Standing)
		{
			SetDesiredStance(AlsStanceTags::Crouching);
		}
	}
	else
	{
		if (GetDesiredStance() == AlsStanceTags::Standing)
		{
			SetDesiredStance(AlsStanceTags::Crouching);
		}
		else if (GetDesiredStance() == AlsStanceTags::Crouching)
		{
			SetDesiredStance(AlsStanceTags::Standing);
		}
	}
}

void AAliCharacter::Input_OnJump(const FInputActionValue& ActionValue)
{
	if (ActionValue.Get<bool>())
	{
		if (StopRagdolling())
			return;

		if (IsValid(ParkourComp) && ParkourComp->TryVaultOver())
			return;

		if (StartMantlingGrounded())
			return;

		if (GetStance() == AlsStanceTags::Crouching)
		{
			SetDesiredStance(AlsStanceTags::Standing);
			return;
		}

		if (IsValid(CoverComponent) && CoverComponent->GetCoverBool())
			return;

		//Jump();
	}
	// else
	// {
	// 	StopJumping();
	// }
}

void AAliCharacter::Input_OnAim(const FInputActionValue& ActionValue)
{
	// A held aim input must not re-enable aiming while running or requesting sprint.
	bManualAimRequested = ActionValue.Get<bool>();
	// Grapple selection only enables its target preview; aiming follows the Aim input.
	SetDesiredAiming(bManualAimRequested && !UAimObstructionComponent::IsRunningForAim(this));
}

void AAliCharacter::Input_OnRagdoll()
{
	if (!StopRagdolling())
	{
		StartRagdolling();
	}
}

void AAliCharacter::Input_OnRoll()
{
	const float PlayRate = GetDesiredStance() == AlsStanceTags::Crouching ? 1.5f : 1.3f;

	StartRolling(PlayRate);
}

void AAliCharacter::Input_OnRotationMode()
{
	SetDesiredRotationMode(GetDesiredRotationMode() == AlsRotationModeTags::VelocityDirection
							   ? AlsRotationModeTags::ViewDirection
							   : AlsRotationModeTags::VelocityDirection);
}

// void AAliCharacter::Input_OnAssassination()
// {
// 	//SetViewMode(GetViewMode() == AlsViewModeTags::ThirdPerson ? AlsViewModeTags::FirstPerson : AlsViewModeTags::ThirdPerson);
// }

UAlsMantlingSettings* AAliCharacter::SelectMantlingSettings(EAlsMantlingType MantlingType)
{
	return Super::SelectMantlingSettings_Implementation(MantlingType);
}

bool AAliCharacter::IsMantlingAllowedToStart_Implementation() const
{
	// Nothing needs to be done. Mantle Settings already checks what object
	// types the player can mantle
	return Super::IsMantlingAllowedToStart_Implementation();
}
