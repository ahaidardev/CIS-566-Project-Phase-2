// AliCharacter: PowerWeaponAndViewMode. Included once by AliCharacter.cpp; retains original definition order.

// void AAliCharacter::CheckObsThicknessEvent()
// {
// 	// Call CheckObsThickness with preset parameters
// 	CheckObsThickness(
// 		DrawDebugType,
// 		100.0,  // Forward Distance
// 		28.0f,  // Radius
// 		FLinearColor(0.0f, 0.81f, 1.0f),  // Trace Color (cyan)
// 		FLinearColor(0.0f, 1.0f, 0.88f),  // Trace Hit Color (mint)
// 		4.0f    // Draw Time
// 	);
// }
//
// void AAliCharacter::CheckObsHeightEvent()
// {
// 	// Call CheckObsHeight with preset parameters
// 	CheckObsHeight(
// 		20.0,   // Trace Height
// 		42.0f,  // Trace Forward
// 		FLinearColor(1.0f, 0.98f, 0.1f),  // Trace Color (yellow)
// 		FLinearColor(1.0f, 0.89f, 0.0f),  // Trace Hit Color (orange-yellow)
// 		DrawDebugType
// 	);
// }
//
// void AAliCharacter::CheckObsHeight(double InTraceHeight, float TraceForward, FLinearColor InTraceColor,
// 	FLinearColor InTraceHitColor, TEnumAsByte<EDrawDebugTrace::Type> InDrawDebugType)
// {
// 	// Calculate trace start and end based on character position
// 	FVector CharLocation = GetActorLocation();
// 	FVector CharForward = GetActorForwardVector();
//
// 	FVector HeightTraceStart = CharLocation + CharForward * TraceForward + FVector(0, 0, InTraceHeight);
// 	FVector HeightTraceEnd = CharLocation + CharForward * TraceForward + FVector(0, 0, InTraceHeight + 200.0);
//
// 	// Multi-trace for height detection
// 	TArray<FHitResult> OutHits;
// 	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
// 		this,
// 		HeightTraceStart,
// 		HeightTraceEnd,
// 		10.0f, // Radius
// 		ObjectTypes,
// 		true,  // Trace complex
// 		TArray<AActor*>(),
// 		InDrawDebugType,
// 		OutHits,
// 		true,  // Ignore self
// 		InTraceColor,
// 		InTraceHitColor,
// 		5.0f
// 	);
//
// 	if (bHit)
// 	{
// 		// Process each hit
// 		for (const FHitResult& Hit : OutHits)
// 		{
// 			if (Hit.bBlockingHit)
// 			{
// 				// Create impact point with X/Y from location, Z from impact
// 				ObsHeightImpactPoint = FVector(Hit.Location.X, Hit.Location.Y, Hit.ImpactPoint.Z);
// 				ObsHeightTraceStart = Hit.TraceStart;
// 				ObsHeightTraceEnd = Hit.TraceEnd;
//
// 				// Check if this is a new point (not zero vector)
// 				if (!ObsHeightImpactPoint.IsNearlyZero())
// 				{
// 					ObsHeightImpactPointsArray.Add(ObsHeightImpactPoint);
// 				}
// 			}
// 		}
//
// 		// Add trace end to array
// 		ObsHeightImpactPointsArray.Add(ObsHeightTraceEnd);
// 	}
// }
//
// void AAliCharacter::CheckObsInfo(FVaultMantleInfo& VaultMantleStructParam)
// {
// 	// Extract struct data
// 	StandLocationVector = VaultMantleStructParam.StandLocationVector;
//
// 	// Calculate distance from stand location to actor
// 	Distance = FVector::Dist(StandLocationVector, GetActorLocation());
//
// 	// Check if character is crouching to determine which room check matters
// 	bool bIsCrouching = AlsCharacterMovement->IsCrouching();
//
// 	if (bIsCrouching)
// 	{
// 		if (VaultMantleStructParam.bIsThereRoomForCrouching)
// 		{
// 			// Valid position - add to distances and location map
// 			if (!DistanceLocation.Contains(Distance))
// 			{
// 				Distances.Add(Distance);
// 				DistanceLocation.Add(Distance, StandLocationVector);
// 			}
// 		}
// 	}
// 	else
// 	{
// 		if (VaultMantleStructParam.bIsThereRoomForStanding)
// 		{
// 			// Valid position - add to distances and location map
// 			if (!DistanceLocation.Contains(Distance))
// 			{
// 				Distances.Add(Distance);
// 				DistanceLocation.Add(Distance, StandLocationVector);
// 			}
// 		}
// 	}
// }

// void AAliCharacter::CheckObsGap(TEnumAsByte<EDrawDebugTrace::Type> InDrawDebugType)
// {
// 	// Get last index of impact points array
// 	Index = ObsHeightImpactPointsArray.Num() - 1;
//
// 	// Iterate through impact points
// 	for (int32 i = 0; i < ObsHeightImpactPointsArray.Num(); i++)
// 	{
// 		FVector CurrentPoint = ObsHeightImpactPointsArray[i];
//
// 		// Skip zero vectors
// 		if (CurrentPoint.IsNearlyZero())
// 		{
// 			UE_LOG(LogTemp, Log, TEXT("Caught Empty Value"));
// 			continue;
// 		}
//
// 		// Skip if at first index
// 		if (i <= 0)
// 		{
// 			continue;
// 		}
//
// 		Index = i;
// 		int32 NextIndex = i - 1;
//
// 		// Calculate height difference between consecutive points
// 		if (NextIndex >= 0 && NextIndex < ObsHeightImpactPointsArray.Num())
// 		{
// 			VectorsHeightDifferenceResult = FVector::Dist(
// 				ObsHeightImpactPointsArray[i],
// 				ObsHeightImpactPointsArray[NextIndex]
// 			);
// 		}
//
// 		// Check if character is crouching
// 		bool bIsCrouching = AlsCharacterMovement->IsCrouching();
//
// 		if (bIsCrouching)
// 		{
// 			// Check for crouched gap
// 			CheckObsGapPure(
// 				GetCapsuleComponent(),
// 				0.0,
// 				InDrawDebugType,
// 				FLinearColor(0.37f, 0.93f, 1.0f),
// 				FLinearColor::Black,
// 				FVector(0, 0, 40.0f),
// 				FVector(0, 0, 40.0f),
// 				bHasRoomForCrouched,
// 				CheckGapTraceEnd
// 			);
// 		}
// 		else
// 		{
// 			// Check for standing gap
// 			CheckObsGapPure(
// 				GetCapsuleComponent(),
// 				0.0,
// 				InDrawDebugType,
// 				FLinearColor(0.37f, 0.93f, 1.0f),
// 				FLinearColor::Black,
// 				FVector(0, 0, 40.0f),
// 				FVector(0, 0, 120.0f),
// 				bHasRoomForStand,
// 				CheckGapTraceEnd
// 			);
// 		}
//
// 		// Create VaultMantleInfo struct and add to array
// 		FVaultMantleInfo NewInfo;
// 		NewInfo.Index = i;
// 		NewInfo.Index2 = NextIndex;
// 		NewInfo.StandLocationVector = CheckGapTraceEnd;
// 		NewInfo.bIsThereRoomForCrouching = bHasRoomForCrouched;
// 		NewInfo.bIsThereRoomForStanding = bHasRoomForStand;
//
// 		VaultMantleStruct.Add(NewInfo);
// 	}
// }

// void AAliCharacter::CheckObsGapPure(UCapsuleComponent* Capsule, double RadiusOffset,
// 	TEnumAsByte<EDrawDebugTrace::Type> InDrawDebugType, FLinearColor InTraceColor, FLinearColor InTraceHitColor,
// 	FVector Start, FVector End, bool& bHasRoom, FVector& CheckGapTraceEndOut)
// {
// 	if (!Capsule || Index < 0 || Index >= ObsHeightImpactPointsArray.Num())
// 	{
// 		bHasRoom = false;
// 		return;
// 	}
//
// 	// Get capsule radius and add offset
// 	float CapsuleRadius = Capsule->GetScaledCapsuleRadius();
// 	float TraceRadius = CapsuleRadius + RadiusOffset;
//
// 	// Calculate trace positions based on obstacle impact point
// 	FVector BasePoint = ObsHeightImpactPointsArray[Index];
// 	FVector TraceStart = BasePoint + Start;
// 	FVector LocalTraceEnd = TraceStart + End;
//
// 	// Object types for trace
// 	TArray<TEnumAsByte<EObjectTypeQuery>> GapObjectTypes;
// 	GapObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
//
// 	// Perform sphere trace
// 	FHitResult OutHit;
// 	bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
// 		this,
// 		LocalTraceEnd,
// 		TraceStart,
// 		TraceRadius,
// 		GapObjectTypes,
// 		false,
// 		TArray<AActor*>(),
// 		InDrawDebugType,
// 		OutHit,
// 		true,
// 		InTraceColor,
// 		InTraceHitColor,
// 		10.0f
// 	);
//
// Has room if no blocking hit AND no initial overlap (NOR logic)
// 	bHasRoom = !(OutHit.bBlockingHit || OutHit.bStartPenetrating);
// 	CheckGapTraceEndOut = OutHit.TraceEnd;
// }

// void AAliCharacter::CheckThickness()
// {
// }

// Fs_PlayerSave AAliCharacter::GetPlayerSave()
// {
// 	// Populate PlayerData with current state
// 	PlayerData.Transform = GetActorTransform();
// 	PlayerData.Health = Health;
// 	PlayerData.Stance = GetStance();
// 	PlayerData.ControlRotation = GetControlRotation();
//
// 	return PlayerData;
// }

// void AAliCharacter::UsePlayerSave(Fs_PlayerSave PlayerSave)
// {
// 	// Restore player state from save
// 	SetActorTransform(PlayerSave.Transform);
// 	Health = PlayerSave.Health;
// 	SetDesiredStance(PlayerSave.Stance);
// 	if (APlayerController* PC = Cast<APlayerController>(GetController()))
// 	{
// 		PC->SetControlRotation(PlayerSave.ControlRotation);
// 	}
// }

// bool AAliCharacter::IsMantlingAllowedToStartBP()
// {
// 	return false;
// }
//
// void AAliCharacter::ThrowShockChip(bool bPowerShot, FVector Velocity)
// {
// 	if (UWorld* World = GetWorld())
// 	{
// 		FActorSpawnParameters SpawnParams;
// 		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
//
// 		UClass* ShockChipClass = LoadClass<AActor>(nullptr, TEXT("/Game/OverAndOut/AbilityChips/ShockChip_bp.ShockChip_bp_C"));
// 		if (ShockChipClass)
// 		{
// 			World->SpawnActor<AActor>(ShockChipClass, ProjectileSpawnPoint, SpawnParams);
// 		}
// 	}
// }

// void AAliCharacter::AutoSelect()
// {
// 	// Auto-select next card from card hand
// 	// This requires widget references - calling through Blueprint implementable event pattern
// 	// HudCard->CardHand->GetNextCard() then set SelectedCard
// }

// void AAliCharacter::ThrowGrenade()
// {
// 	// Spawn PyroBlastChip_BP at ProjectileSpawnPoint with FastProjectileVelocity
// 	if (UWorld* World = GetWorld())
// 	{
// 		FActorSpawnParameters SpawnParams;
// 		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
//
// 		UClass* GrenadeClass = LoadClass<AActor>(nullptr, TEXT("/Game/OverAndOut/AbilityChips/PyroBlastChip_BP.PyroBlastChip_BP_C"));
// 		if (GrenadeClass)
// 		{
// 			FTransform SpawnTransformWithVelocity = ProjectileSpawnPoint;
// 			AActor* SpawnedGrenade = World->SpawnActor<AActor>(GrenadeClass, SpawnTransformWithVelocity, SpawnParams);
// 			if (SpawnedGrenade)
// 			{
// 				if (UProjectileMovementComponent* ProjMove = SpawnedGrenade->FindComponentByClass<UProjectileMovementComponent>())
// 				{
// 					ProjMove->Velocity = FastProjectileVelocity;
// 				}
// 			}
// 		}
// 	}
// }

// void AAliCharacter::HairLODSetup()
// {
// 	// Check if hair strands rendering is enabled
// 	IConsoleVariable* HairStrandsVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.HairStrands.Strands"));
// 	if (HairStrandsVar && HairStrandsVar->GetInt() != 0)
// 	{
// 		// Hair strands enabled - skip LOD setup
// 		return;
// 	}
//
// 	// Get material at index 10 for hair fallback
// 	UMaterialInterface* HairMaterial = GetMesh()->GetMaterial(10);
// 	if (UMaterialInstanceDynamic* HairDynMat = Cast<UMaterialInstanceDynamic>(HairMaterial))
// 	{
// 		// Get current opacity values
// 		float Opacity = 0.0f;
// 		float OpacityRT = 0.0f;
// 		HairDynMat->GetScalarParameterValue(TEXT("Opacity"), Opacity);
// 		HairDynMat->GetScalarParameterValue(TEXT("OpacityRT"), OpacityRT);
//
// 		// Get material at index 6 and apply the opacity values
// 		UMaterialInterface* TargetMaterial = GetMesh()->GetMaterial(6);
// 		if (UMaterialInstanceDynamic* TargetDynMat = Cast<UMaterialInstanceDynamic>(TargetMaterial))
// 		{
// 			TargetDynMat->SetScalarParameterValue(TEXT("Opacity"), Opacity);
// 			TargetDynMat->SetScalarParameterValue(TEXT("OpacityRT"), OpacityRT);
// 		}
// 	}
// }

// void AAliCharacter::DiscardSelectedCard()
// {
// 	// Play discard sound
// 	UGameplayStatics::PlaySound2D(
// 		this,
// 		LoadObject<USoundBase>(nullptr, TEXT("/Game/OverAndOut/Player/Sounds/WWise/DRAFT_Discard_1_Loud.DRAFT_Discard_1_Loud")),
// 		1.0f,
// 		1.0f,
// 		0.0f,
// 		nullptr,
// 		nullptr,
// 		true // UI sound
// 	);
//
// 	// Card discard logic requires widget references
// 	// HudCard->CardHand->SelectedCard->UseCard()
// 	// Then GetPreviousCard and SelectCard
// }

void AAliCharacter::PowerWeaponShot()
{
	// Perform weapon aim trace first
	FVector TraceStart, TraceEndOut;
	FHitResult OutHit;
	WeaponAimTrace(TraceStart, TraceEndOut, OutHit);

	// Switch based on current card type
	// if (CurrentCard == TEXT("Fire"))
	// {
	// 	ThrowGrenade();
	// }
}

// void AAliCharacter::ThrowSmoke()
// {
// 	// Use smoke bomb via stealth component
// 	// AbilityComponent->UseSmokeBomb()
// }

// void AAliCharacter::TraceReq()
// {
// 	// Call the sequence of trace checks for vault/mantle system
// 	CheckObsThicknessEvent();
// 	CheckObsHeightEvent();
// 	CheckGapSizeEvent(); // Blueprint implementable event
// }

// void AAliCharacter::EnableElectroShield()
// {
// 	// TODO: Enable the lightning shield Niagara effect
// }
//
// void AAliCharacter::DisableElectricShield()
// {
// 	// TODO: Disable the lightning shield Niagara effect
// }

// void AAliCharacter::EquipCard()
// {
// }

// void AAliCharacter::AbsorbShard()
// {
// 	// Switch based on CurrentCard string
// 	// if (CurrentCard == TEXT("Shock"))
// 	// {
// 	// 	EnableElectroShield();
// 	// }
// 	// else if (CurrentCard == TEXT("Smoke"))
// 	// {
// 	// 	EnableInvisibility();
// 	// }
// 	// else if (CurrentCard == TEXT("Rock"))
// 	// {
// 	// 	ThrowCloneChip();
// 	// }
//
// 	// Discard the selected card after absorbing
// 	DiscardSelectedCard();
// }

// void AAliCharacter::EnableInvisibility()
// {
// 	// Conceal owner via stealth component
// 	// Call ConcealOwner on the StealthComponent
// 	// Then set timer to DisableInvisibility after InvisibilityTime seconds
//
// 	FTimerHandle InvisibilityTimerHandle;
// 	GetWorldTimerManager().SetTimer(
// 		InvisibilityTimerHandle,
// 		this,
// 		&AAliCharacter::DisableInvisibility,
// 		InvisibilityTime,
// 		false
// 	);
//
// 	// Call the Blueprint Implementable Event
// 	Conceal();
// }
//not required
// bool AAliCharacter::GrappleGetTargetInfo()
// {
// 	return false;
// }
//
// bool AAliCharacter::GrappleIdentifyTarget()
// {
// 	return false;
// }
//
// void AAliCharacter::GrappleCalculatePlayerGrappleTargetLocation()
// {
// }
//
// void AAliCharacter::GrappleSetActorLocation(float Alpha)
// {
// }

// void AAliCharacter::HideMeshFx(float Appearance)
// {
// 	// Set material parameters on all dynamic materials for invisibility effect
//
// 	// Loop through AliDynMatRef array and set parameters
// 	for (UMaterialInstanceDynamic* DynMat : AliDynMatRef)
// 	{
// 		if (DynMat)
// 		{
// 			DynMat->SetScalarParameterValue(TEXT("Appearance"), Appearance);
// 			DynMat->SetScalarParameterValue(TEXT("Offset_2"), 0.0f);
// 			DynMat->SetScalarParameterValue(TEXT("Offset_1"), 0.0f);
// 		}
// 	}
//
// 	// Set parameters on AliDynHair (with multiplied appearance)
// 	if (AliDynHair)
// 	{
// 		AliDynHair->SetScalarParameterValue(TEXT("Appearance"), Appearance * 4.0f);
// 		AliDynHair->SetScalarParameterValue(TEXT("Offset_2"), 0.0f);
// 		AliDynHair->SetScalarParameterValue(TEXT("Offset_1"), 0.0f);
// 	}
//
// 	// Set parameters on AliDynMask
// 	if (AliDynMask)
// 	{
// 		AliDynMask->SetScalarParameterValue(TEXT("Appearance"), Appearance);
// 		AliDynMask->SetScalarParameterValue(TEXT("Offset_2"), 0.0f);
// 		AliDynMask->SetScalarParameterValue(TEXT("Offset_1"), 0.0f);
// 	}
// }

// void AAliCharacter::DisableInvisibility()
// {
// 	// Reveal owner via stealth component - call the Blueprint Implementable Event
// 	Reveal();
// }

void AAliCharacter::Input_OnViewMode()
{
	// Toggle between first and third person view modes
	SetViewMode(GetViewMode() == AlsViewModeTags::ThirdPerson ? AlsViewModeTags::FirstPerson
															  : AlsViewModeTags::ThirdPerson);
}

// void AAliCharacter::UpdateThrowableTrajectory() -->probably not needed
// {
// }

// void AAliCharacter::PerformLineTrace(const FVector& Start, const FVector& End, FHitResult& OutHit)
// {
// 	FCollisionQueryParams QueryParams;
// 	QueryParams.AddIgnoredActor(this);
//
// 	GetWorld()->LineTraceSingleByChannel(
// 		OutHit,
// 		Start,
// 		End,
// 		ECC_Visibility,
// 		QueryParams
// 	);
// }
//
// void AAliCharacter::HandleCoverMovement(float DeltaTime)
// {
// }
//
// void AAliCharacter::UpdateVaultingState()
// {
// }

//
// bool AAliCharacter::OnMantlingStarted_Implementation()
// {
// 	return false;
// }

//}
//
// bool AAliCharacter::OnMantlingStarted_Implementation()
// {
//
