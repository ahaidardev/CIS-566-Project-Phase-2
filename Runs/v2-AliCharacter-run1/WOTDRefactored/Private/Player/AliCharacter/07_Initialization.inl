// AliCharacter: Initialization. Included once by AliCharacter.cpp; retains original definition order.

//
// void AAliCharacter::SetOverlayMode(const FGameplayTag& NewOverlayMode, const bool bSendRpc)
//   {
//   	if (OverlayMode == NewOverlayMode || GetLocalRole() <= ROLE_SimulatedProxy)
//   	{
//   		return;
//   	}

// 	const auto PreviousOverlayMode{OverlayMode};
//
// 	OverlayMode = NewOverlayMode;
//
// 	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, OverlayMode, this)
//
//  OnOverlayModeChanged(PreviousOverlayMode);
//
// 	if (bSendRpc)
// 	{
// 		if (GetLocalRole() >= ROLE_Authority)
// 		{
// 			// ClientSetOverlayMode(OverlayMode);
// 		}
// 		else
// 		{
// 			// ServerSetOverlayMode(OverlayMode);
// 		}
// 	}
// }
//
// void AAliCharacter::OnReplicated_OverlayMode(const FGameplayTag& PreviousOverlayMode)
//   {
//   	OnOverlayModeChanged(PreviousOverlayMode);
//   }
//void AAlsCharacter::OnOverlayModeChanged_Implementation(const FGameplayTag& PreviousOverlayMode) {}

void AAliCharacter::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& Unused,
								 float& VerticalLocation)
{
	if (Camera->IsActive())
	{
		Camera->DisplayDebug(Canvas, DisplayInfo, VerticalLocation);
	}

	Super::DisplayDebug(Canvas, DisplayInfo, Unused, VerticalLocation);
}

void AAliCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	// TODO: Impelement Felloutofworld
}

void AAliCharacter::PostInitializeComponents()
{
	if (IsValid(MovementSettings))
	{
		// ALS settings are shared assets; tune a private copy for this player.
		MovementSettings = DuplicateObject<UAlsMovementSettings>(MovementSettings, this);
		for (auto& RotationEntry : MovementSettings->RotationModes)
		{
			if (FAlsMovementGaitSettings* Crouch = RotationEntry.Value.Stances.Find(AlsStanceTags::Crouching))
			{
				Crouch->WalkForwardSpeed *= 1.25f;
				Crouch->WalkBackwardSpeed *= 1.25f;
				Crouch->RunForwardSpeed *= 1.25f;
				Crouch->RunBackwardSpeed *= 1.25f;
				Crouch->SprintSpeed *= 1.25f;
			}
		}
	}
	Super::PostInitializeComponents();
}

void AAliCharacter::NotifyLocomotionActionChanged(const FGameplayTag& PreviousLocomotionAction)
{
	if (PlayerSFXComponent && PlayerSFXComponent->HasBegunPlay() && GetLocomotionAction() != PreviousLocomotionAction)
	{
		if (GetLocomotionAction() == AlsLocomotionActionTags::Mantling)
			PlayerSFXComponent->RequestVoice(EPlayerVoiceScenario::Mantling);
		else if (GetLocomotionAction() == AlsLocomotionActionTags::Rolling)
			PlayerSFXComponent->RequestVoice(EPlayerVoiceScenario::Rolling);
	}
	const bool bStartingRoll = GetLocomotionAction() == AlsLocomotionActionTags::Rolling;
	const bool bEndingRoll = PreviousLocomotionAction == AlsLocomotionActionTags::Rolling;
	if (bStartingRoll)
	{
		bRollingStartedCrouched = GetDesiredStance() == AlsStanceTags::Crouching;
		RootMotionScaleBeforeRoll = GetAnimRootMotionTranslationScale();
		if (bRollingStartedCrouched)
		{
			SetAnimRootMotionTranslationScale(RootMotionScaleBeforeRoll * 1.25f);
		}
	}
	else if (bEndingRoll)
	{
		SetAnimRootMotionTranslationScale(RootMotionScaleBeforeRoll);
		if (!GetLocomotionAction().IsValid())
		{
			if (bRollingStartedCrouched)
			{
				SetDesiredStance(AlsStanceTags::Crouching);
			}
		}
	}

	Super::NotifyLocomotionActionChanged(PreviousLocomotionAction);

	if (bEndingRoll && !bStartingRoll)
	{
		bRollingStartedCrouched = false;
	}
}

void AAliCharacter::BeginPlay()
{
	ALS_ENSURE(IsValid(Settings));
	ALS_ENSURE(IsValid(MovementSettings));
	if (!ALS_ENSURE(AnimationInstance.IsValid()))
	{
		return;
	}

	ALS_ENSURE_MESSAGE(!bUseControllerRotationPitch && !bUseControllerRotationYaw && !bUseControllerRotationRoll,
					   TEXT("These settings are not allowed and must be turned off!"));

	Super::BeginPlay();
	InitializeDynamicMaterials();

	// if(IsValid(HitGrid))
	// 	GetWorld()->SpawnActor(HitGrid->GetClass());

	/* 
	* If using a blueprint version of the cover component to override functions, 
	* comment out the create default subobject from constructor
	* and simply uncomment this line.
	*/
	//CoverComponent = FindComponentByClass<UCoverComponent>();

	// if (GetLocalRole() >= ROLE_AutonomousProxy)
	// {
	// 	// Teleportation of simulated proxies is detected differently, see
	// 	// AAlsCharacter::PostNetReceiveLocationAndRotation() and AAlsCharacter::OnRep_ReplicatedBasedMovement().
	//
	// 	GetCapsuleComponent()->TransformUpdated.AddWeakLambda(
	//   this, [this](USceneComponent*, const EUpdateTransformFlags, const ETeleportType TeleportType)
	//   {
	// 	  if (TeleportType != ETeleportType::None && AnimationInstance.IsValid())
	// 	  {
	// 		  AnimationInstance->MarkTeleported();
	// 	  }
	//   });
	// }
}
