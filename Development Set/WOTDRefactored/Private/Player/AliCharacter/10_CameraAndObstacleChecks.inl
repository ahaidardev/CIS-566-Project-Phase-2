// AliCharacter: CameraAndObstacleChecks. Included once by AliCharacter.cpp; retains original definition order.

//Not Used
// void AAliCharacter::ThrowRock()
// {
// 	if (UWorld* World = GetWorld())
// 	{
// 		FActorSpawnParameters SpawnParams;
// 		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
//
// 		// Spawn SandChip_bp at ProjectileSpawnPoint
// 		UClass* RockClass = LoadClass<AActor>(nullptr, TEXT("/Game/OverAndOut/Misc/Cards/SandChip_bp.SandChip_bp_C"));
// 		if (RockClass)
// 		{
// 			AActor* SpawnedRock = World->SpawnActor<AActor>(RockClass, ProjectileSpawnPoint, SpawnParams);
// 			if (SpawnedRock)
// 			{
// 				if (UProjectileMovementComponent* ProjMove = SpawnedRock->FindComponentByClass<UProjectileMovementComponent>())
// 				{
// 					ProjMove->Velocity = FastProjectileVelocity;
// 				}
// 			}
// 		}
// 	}
// }

// void AAliCharacter::UseSelectedCard()
// {
// 	// TODO: Implement card usage - requires HudCard and CardHand widget references
// }
//
// FString AAliCharacter::GetCardName(UUserWidget* self2)
// {
// 	// Return the current card name
// 	return CurrentCard;
// }

// void AAliCharacter::VaultMotionWarp()
// {
// 	// Motion warping for vault - requires MotionWarpingComponent setup
// 	// The actual warp target is set via VaultMantleStruct data
// }
//
// void AAliCharacter::CameraChanges() --> implement in blueprints
// {
// 	if (!Camera) return;
//
// 	// Check if crouching to determine target FOV
// 	bool bIsCrouching = AlsCharacterMovement->IsCrouching();
//
// 	if (bIsCrouching)
// 	{
// 		// Set crouched FOV (75)
// 		Camera->SetFOV(75.0f);
// 	}
// 	else
// 	{
// 		// Set standing FOV (90)
// 		Camera->SetFOV(90.0f);
// 	}
// }

// void AAliCharacter::RefreshOverlayLinkedAnimationLayer()
// {
// }
//---------------------OBJ CHECKING BELONGS IN COVER AND PARKOUR COMPS--------------------------------------------
// 		bool bVaultWallExists;
//
// 		FLinearColor TraceColor;
//
// 		float DrawTime;
//
// 		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
//
// 		FVector_NetQuantize ObsExistImpactPoint;
//
// 		FRotator TargetRotation;
//
// 		bool bDidHit;

void AAliCharacter::SetCameraRightShoulder(bool bRightShoulder)
{
	if (Camera)
	{
		Camera->SetRightShoulder(bRightShoulder);
	}
}

bool AAliCharacter::GetCameraRightShoulder()
{
	return Camera->IsRightShoulder();
}

void AAliCharacter::CheckIfObjectExists(bool& bOutObstacleExists, FRotator& OutTargetRotation)
{
	// temporary stub - full impl once trace vars are sorted
	bOutObstacleExists = false;
	OutTargetRotation = FRotator::ZeroRotator;

	// 	// Determine trace height based on crouching state
	// 	float TraceHeight = AlsCharacterMovement->IsCrouching() ? -22.0f : -35.0f;
	//
	// 	// Calculate capsule trace start/end based on character position and forward direction
	// 	FVector CharLocation = GetActorLocation();
	// 	FVector CharForward = GetActorForwardVector();
	//
	// 	// First trace to check if vault wall exists
	// 	FVector WallTraceStart = CharLocation + FVector(0, 0, TraceHeight);
	// 	FVector WallTraceEnd = CharLocation + CharForward * 100.0f + FVector(0, 0, TraceHeight);
	//
	// 	FHitResult WallHit;
	// 	bool bWallHit = UKismetSystemLibrary::CapsuleTraceSingle(
	// 		this,
	// 		WallTraceStart,
	// 		WallTraceEnd,
	// 		50.0f, // Radius
	// 		0.0f,  // Half Height
	// 		ETraceTypeQuery::TraceTypeQuery3, // Wall channel
	// 		false, // Trace complex
	// 		TArray<AActor*>(),
	// 		DrawDebugType,
	// 		WallHit,
	// 		true, // Ignore self
	// 		TraceColor,
	// 		FLinearColor::Green,
	// 		DrawTime
	// 	);
	//
	// 	bVaultWallExists = bWallHit;
	//
	// 	if (bVaultWallExists) //--> implement
	// 	{
	// 		// // Early return - can't vault if wall in the way
	// 		// bOutObstacleExists = false;
	// 		// //OutTargetRotation = TargetRotation;
	// 		// return;
	// 	}
	//
	// 	// Second trace to check for obstacle
	// 	FVector ObstacleTraceStart = CharLocation + FVector(0, 0, TraceHeight);
	// 	FVector ObstacleTraceEnd = CharLocation + CharForward * 150.0f + FVector(0, 0, TraceHeight);
	//
	// 	FHitResult ObstacleHit;
	// 	bool bObstacleHit = UKismetSystemLibrary::CapsuleTraceSingleForObjects(
	// 		this,
	// 		ObstacleTraceStart,
	// 		ObstacleTraceEnd,
	// 		45.0f, // Radius
	// 		46.0f, // Half Height
	// 		ObjectTypes,
	// 		false,
	// 		TArray<AActor*>(),
	// 		DrawDebugType,
	// 		ObstacleHit,
	// 		true,
	// 		TraceColor,
	// 		FLinearColor::Green,
	// 		DrawTime
	// 	);
	//
	// 	bDidHit = bObstacleHit;
	//
	// 	if (bDidHit)
	// 	{
	// 		ObsExistImpactPoint = ObstacleHit.ImpactPoint;
	// 		ActorRotation = GetActorRotation();
	// 		TargetRotation = UKismetMathLibrary::FindRelativeLookAtRotation(GetActorTransform(), ObsExistImpactPoint);
	// 		bIsFalling = AlsCharacterMovement->IsFalling();
	// 	}
	//
	// 	// Check if montage is playing
	// 	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	// 	bool bMontageNotPlaying = AnimInst ? !AnimInst->Montage_IsPlaying(AnimMontage) : true;
	//
	// 	bObstacleExists = bDidHit && bMontageNotPlaying;
	// 	bOutObstacleExists = bObstacleExists;
	// 	OutTargetRotation = TargetRotation;
}
