// AliCharacter: InteractionTracing. Included once by AliCharacter.cpp; retains original definition order.

void AAliCharacter::KO()
{
	// KO functionality - trigger knockout animation or state
	// This is typically called after a successful assassination or knockout attack
}

void AAliCharacter::ItemTrace(UActorComponent*& StealthComponent, AActor*& OutHitActor)
{
	// Get player capsule location and forward vector
	APawn* PlayerPawn = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!PlayerPawn)
		return;

	UCapsuleComponent* PlayerCapsule = PlayerPawn->FindComponentByClass<UCapsuleComponent>();
	if (!PlayerCapsule)
		return;

	FVector CapsuleLocation = PlayerCapsule->GetComponentLocation();
	FVector ForwardVec = PlayerCapsule->GetForwardVector();

	FVector LocalTraceStart = CapsuleLocation + ForwardVec * 10.0f;
	FVector LocalTraceEnd = CapsuleLocation;

	// Capsule trace for interactable objects
	TArray<FHitResult> OutHits;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	UKismetSystemLibrary::CapsuleTraceMultiForObjects(
		this, LocalTraceStart, LocalTraceEnd, 500.0f, 50.0f, InteractableObjectTypes, false, TArray<AActor*>(),
		DrawDebugType, OutHits, true, FLinearColor(0.93f, 0.98f, 1.0f), FLinearColor::Green, 5.0f);

	// Check each hit for interaction interface
	for (const FHitResult& Hit : OutHits)
	{
		AActor* LocalHitActor = Hit.GetActor();
		if (LocalHitActor && LocalHitActor->GetClass()->ImplementsInterface(UInterface::StaticClass()))
		{
			OutHitActor = LocalHitActor;
			// Get stealth component if available
			// StealthComponent = ...
			return;
		}
	}
}

ETraceTypeQuery TraceChannel;

bool AAliCharacter::EnemyTrace(FHitResult& OutHit)
{
	// Get player character and capsule
	APawn* PlayerPawn = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!PlayerPawn)
		return false;

	UCapsuleComponent* PlayerCapsule = PlayerPawn->FindComponentByClass<UCapsuleComponent>();
	if (!PlayerCapsule)
		return false;

	// Get player transform and forward direction
	FVector PlayerLocation = PlayerPawn->GetActorTransform().GetLocation();
	FRotator CapsuleRotation = PlayerCapsule->GetComponentRotation();
	FVector ForwardVec = UKismetMathLibrary::GetForwardVector(CapsuleRotation);

	// Calculate trace start and end
	FVector EnemyTraceStart = PlayerLocation + (ForwardVec * 10.0f);
	FVector EnemyTraceEnd = PlayerLocation + (ForwardVec * 100.0f);

	// Perform sphere trace on Assassination channel
	bool bHit = UKismetSystemLibrary::SphereTraceSingle(this, EnemyTraceStart, EnemyTraceEnd,
														0.0f,		  // Radius
														TraceChannel, // Assassination trace channel
														false,		  // Trace complex
														TArray<AActor*>(), DrawDebugType, OutHit,
														true, // Ignore self
														FLinearColor::Red, FLinearColor::Green, 5.0f);

	return bHit;
}
