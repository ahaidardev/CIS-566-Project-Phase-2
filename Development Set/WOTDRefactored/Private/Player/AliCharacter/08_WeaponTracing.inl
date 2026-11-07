// AliCharacter: WeaponTracing. Included once by AliCharacter.cpp; retains original definition order.

// void AAliCharacter::Landed(const FHitResult& Hit)
// {
// 	Super::Landed(Hit);
// }

// void AAliCharacter::AddToInventory(uint8 Type)
// {
// 	// TODO: Implement inventory addition based on Type enum
// }
//
// void AAliCharacter::RemoveFromInventory()
// {
// 	// TODO: Implement inventory removal
// }

// void AAliCharacter::SpawnProjectile(FVector Velocity, double Gravity)
// {
// 	// TODO: Implement projectile spawning with given velocity and gravity

// }

// {// void AAliCharacter::SetAimingVariables(double Gravity, FVector Velocity)

// # AIMING (Triggered by EnhancedInputAction IA_Als_Aim)
// Event IA_Als_Aim (Started/Ongoing):
//   Set Desired Aiming (true)  // Lerp camera FOV/zoom, play aim anim timeline
//   Update Timeline (Aim Blend)  // Smooth aim rotation/position
//   Get CT_Cmpt_Crosshair → Capsule Trace For Objects (screen center → world point)
// 	→ Set CanTrace? (hit check for cover/occlusion)
// 	→ Get Trace Point "Target is CT Cmpt Crosshair"
// 	→ Predict Projectile Path (Advanced)  // Spline arc preview?
// 	→ Calculate Projectile Spawn (muzzle bone)
// 	→ Suggest Projectile Velocity Custom Arc (gravity-compensated velocity to trace hit)
// Event IA_Als_Aim (Completed/Canceled) → CustomEvent "Stop Aiming":
//   Set Desired Aiming (false)
//   Stop Timelines (Aim Blend)
// # SHOOTING (Likely separate IA_Fire Triggered → Branches Normal/Power)
// Event Fire Input (Triggered):
//   Branch on CanTrace? / Power Mode:
// 	True → Power Weapon Shot:
// 	  Get ProjectileSpawnPoint (muzzle bone)
// 	  Set Fast Projectile Velocity (from arc prediction)
// 	  Spawn Projectile (arc path to trace point)
// 	False → Normal Weapon Shot:
// 	  Similar spawn/velocity but linear/direct
// # Shared Helpers:
// - Cover Line Trace Setup (CapsuleTrace → Crouch/Adjust aim offset)
// - Splines for path viz (On Splines Cleared → Bind/Spawn spline actors for preview)
// - No recoil/muzzle FX nodes visible—likely in anim BP or child functions
// }

// void AAliCharacter::UpdateInventoryUI()
// {
// 	// TODO: Implement UI update for inventory
// }
//
// void AAliCharacter::SetShotPower(double Strength, bool NewParam)
// {
// 	PlayerThrowStrength = Strength;
// }

// void AAliCharacter::FirePowerShotEnd()
// {
// 	bIsFiringCard = true;
//
// 	// If aiming, set desired aiming to true
// 	if (bIsAiming)
// 	{
// 		SetDesiredAiming(true);
// 	}
// }

void AAliCharacter::NormalWeaponShot()
{
	// Switch based on current card type
	// if (CurrentCard == TEXT("Teleport"))
	// {
	// 	ThrowTeleportChip();
	// }
	// else if (CurrentCard == TEXT("Fire"))
	// {
	// 	ThrowFireChip();
	// }
	// else if (CurrentCard == TEXT("Tranquilizer"))
	// {
	// 	ThrowTranq(FastProjectileVelocity);
	// }
	// else if (CurrentCard == TEXT("Clone"))
	// {
	// 	ThrowCloneChip();
	// }
	// else if (CurrentCard == TEXT("Rock"))
	// {
	// 	ThrowRock();
	// }
	// else if (CurrentCard == TEXT("Shock"))
	// {
	// 	ThrowShockChip(false, FastProjectileVelocity);
	// }
}

void AAliCharacter::PulseShotFunction(bool NewParam)
{
	// TODO: Implement pulse shot function
}

EDrawDebugTrace::Type DrawDebugType;

void AAliCharacter::WeaponAimTrace(FVector& TraceStart, FVector& InTraceEnd, FHitResult& OutHit)
{
	// // Get hand_r socket location and rotation from mesh
	// USkeletalMeshComponent* CharMesh = GetMesh();
	// if (!CharMesh) return;
	//
	// FVector HandLocation = CharMesh->GetSocketLocation(TEXT("hand_r"));
	// FRotator HandRotation = CharMesh->GetSocketRotation(TEXT("hand_r"));
	//
	// // Calculate trace direction from hand rotation's right vector
	// FVector RightVector = UKismetMathLibrary::GetRightVector(HandRotation);
	// FVector TraceDirection = RightVector * 1000000.0f;
	//
	// // Set trace start and end
	// TraceStart = HandLocation;
	// FVector LocalTraceEnd = HandLocation + TraceDirection;
	//
	// // Perform line trace
	// bool bHit = UKismetSystemLibrary::LineTraceSingle(
	// 	this,
	// 	TraceStart,
	// 	LocalTraceEnd,
	// 	ETraceTypeQuery::TraceTypeQuery1, // Visibility
	// 	false, // Trace complex
	// 	TArray<AActor*>(),
	// 	DrawDebugType,
	// 	OutHit,
	// 	true, // Ignore self
	// 	FLinearColor(0.0f, 0.05f, 1.0f),
	// 	FLinearColor::Green,
	// 	5.0f

	// Output trace end from hit result
	InTraceEnd = OutHit.TraceEnd;
}

// void AAliCharacter::ThrowTeleportChip()
// {
// 	// Spawn TeleportChip_BP at ProjectileSpawnPoint with FastProjectileVelocity
// 	if (UWorld* World = GetWorld())
// 	{
// 		FActorSpawnParameters SpawnParams;
// 		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
//
// 		// Load TeleportChip_BP class
// 		UClass* TeleportChipClass = LoadClass<AActor>(nullptr, TEXT("/Game/OverAndOut/AbilityChips/TeleportChip_BP.TeleportChip_BP_C"));
// 		if (TeleportChipClass)
// 		{
// 			AActor* SpawnedChip = World->SpawnActor<AActor>(TeleportChipClass, ProjectileSpawnPoint, SpawnParams);
// 			if (SpawnedChip)
// 			{
// 				// Set velocity on the projectile movement component
// 				if (UProjectileMovementComponent* ProjMove = SpawnedChip->FindComponentByClass<UProjectileMovementComponent>())
// 				{
// 					ProjMove->Velocity = FastProjectileVelocity;
// 				}
// 			}
// 		}
// 	}
// }

// void AAliCharacter::ThrowFireChip()
// {
// 	if (UWorld* World = GetWorld())
// 	{
// 		FActorSpawnParameters SpawnParams;
// 		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
//
// 		UClass* FireChipClass = LoadClass<AActor>(nullptr, TEXT("/Game/OverAndOut/AbilityChips/FireChip_BP.FireChip_BP_C"));
// 		if (FireChipClass)
// 		{
// 			AActor* SpawnedChip = World->SpawnActor<AActor>(FireChipClass, ProjectileSpawnPoint, SpawnParams);
// 			if (SpawnedChip)
// 			{
// 				if (UProjectileMovementComponent* ProjMove = SpawnedChip->FindComponentByClass<UProjectileMovementComponent>())
// 				{
// 					ProjMove->Velocity = FastProjectileVelocity;
// 				}
// 			}
// 		}
// 	}
// }
//
// void AAliCharacter::ThrowTranq(FVector Velocity)
// {
// 	if (UWorld* World = GetWorld())
// 	{
// 		FActorSpawnParameters SpawnParams;
// 		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
//
// 		// Spawn TranqThrowable_BP at ProjectileSpawnPoint
// 		UClass* TranqClass = LoadClass<AActor>(nullptr, TEXT("/Game/OverAndOut/AbilityChips/TranqThrowable_BP.TranqThrowable_BP_C"));
// 		if (TranqClass)
// 		{
// 			AActor* SpawnedTranq = World->SpawnActor<AActor>(TranqClass, ProjectileSpawnPoint, SpawnParams);
// 			if (SpawnedTranq)
// 			{
// 				// Set velocity on the projectile movement component
// 				if (UProjectileMovementComponent* ProjMove = SpawnedTranq->FindComponentByClass<UProjectileMovementComponent>())
// 				{
// 					ProjMove->Velocity = Velocity;
// 				}
// 			}
// 		}
// 	}
// }

// void AAliCharacter::ThrowCloneChip()
// {
// 	if (UWorld* World = GetWorld())
// 	{
// 		FActorSpawnParameters SpawnParams;
// 		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
//
// 		UClass* CloneChipClass = LoadClass<AActor>(nullptr, TEXT("/Game/OverAndOut/AbilityChips/Clone_BP.Clone_BP_C"));
// 		if (CloneChipClass)
// 		{
// 			World->SpawnActor<AActor>(CloneChipClass, ProjectileSpawnPoint, SpawnParams);
// 		}
// 	}
// }

void AAliCharacter::CalculateProjectileSpawn(FVector InTraceEnd)
{
	// Calculate projectile spawn point from camera view and mesh socket
	if (Camera)
	{
		FMinimalViewInfo ViewInfo;
		Camera->GetViewInfo(ViewInfo);

		// Set spawn rotation from camera view
		ProjectileSpawnPoint.SetRotation(FQuat(ViewInfo.Rotation));
	}
}
