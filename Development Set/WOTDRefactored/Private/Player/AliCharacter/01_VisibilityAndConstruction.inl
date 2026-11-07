// AliCharacter: VisibilityAndConstruction. Included once by AliCharacter.cpp; retains original definition order.

UAISense_Sight::EVisibilityResult AAliCharacter::CanBeSeenFrom(
	const FCanBeSeenFromContext& Context, FVector& OutSeenLocation, int32& OutNumberOfLoSChecksPerformed,
	int32& OutNumberOfAsyncLosCheckRequested, float& OutSightStrength, int32* UserData,
	const FOnPendingVisibilityQueryProcessedDelegate* Delegate)
{
	OutNumberOfLoSChecksPerformed = 0;
	OutNumberOfAsyncLosCheckRequested = 0;
	OutSightStrength = 0.f;
	OutSeenLocation = GetActorLocation();
	const UAbilitySystemComponent* AbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(this);
	if (!GetWorld() || (AbilitySystem && AbilitySystem->HasMatchingGameplayTag(TAG_Stealth_Player_Status_Concealed)))
	{
		return UAISense_Sight::EVisibilityResult::NotVisible;
	}
	FCollisionQueryParams Params(SCENE_QUERY_STAT(CardVisibility), true, Context.IgnoreActor);
	FHitResult Hit;
	++OutNumberOfLoSChecksPerformed;
	const bool bBlocked =
		GetWorld()->LineTraceSingleByChannel(Hit, Context.ObserverLocation, OutSeenLocation, ECC_Visibility, Params);
	const bool bVisible = !bBlocked || Hit.GetActor() == this || (Hit.GetActor() && Hit.GetActor()->IsOwnedBy(this));
	OutSightStrength = bVisible ? 1.f : 0.f;
	return bVisible ? UAISense_Sight::EVisibilityResult::Visible : UAISense_Sight::EVisibilityResult::NotVisible;
}

AAliCharacter ::AAliCharacter()
{
	Camera = CreateDefaultSubobject<UAlsCameraComponent>(FName{TEXTVIEW("Camera")});
	Camera->SetupAttachment(GetMesh());
	Camera->SetRelativeRotation_Direct({0.0f, 90.0f, 0.0f});

	InventoryComponent = CreateDefaultSubobject<UInventory>(FName{TEXTVIEW("InventoryComponent")});
	GSCCoreComp = CreateDefaultSubobject<UGSCCoreComponent>(FName{TEXTVIEW("GSCCORECOMPONENT")});
	AbilitySystemComponent =
		CreateDefaultSubobject<UGSCAbilitySystemComponent>(FName{TEXTVIEW("AbilitySystemComponent")});
	ParkourComp = CreateDefaultSubobject<UParkourComponent>(FName{TEXTVIEW("ParkourComponent")});
	CoverComponent = CreateDefaultSubobject<UCoverComponent>(FName{TEXTVIEW("CoverComponent")});
	AimObstructionComponent = CreateDefaultSubobject<UAimObstructionComponent>(TEXT("AimObstructionComponent"));
	PlayerSFXComponent = CreateDefaultSubobject<UPlayerSFXComponent>(TEXT("PlayerSFXComponent"));
	CASComponent = CreateDefaultSubobject<UContextualAnimSceneActorComponent>(FName{TEXTVIEW("CASComponent")});
}
void AAliCharacter::NotifyControllerChanged()
{
	const auto* PreviousPlayer{Cast<APlayerController>(PreviousController)};
	if (IsValid(PreviousPlayer))
	{
		auto* InputSubsystem{
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PreviousPlayer->GetLocalPlayer())};
		if (IsValid(InputSubsystem))
		{
			InputSubsystem->RemoveMappingContext(InputMappingContext);
		}
	}

	auto* NewPlayer{Cast<APlayerController>(GetController())};
	if (IsValid(NewPlayer))
	{
		NewPlayer->InputYawScale_DEPRECATED = 1.0f;
		NewPlayer->InputPitchScale_DEPRECATED = 1.0f;
		NewPlayer->InputRollScale_DEPRECATED = 1.0f;

		auto* InputSubsystem{
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NewPlayer->GetLocalPlayer())};
		if (IsValid(InputSubsystem))
		{
			FModifyContextOptions Options;
			Options.bNotifyUserSettings = true;

			InputSubsystem->AddMappingContext(InputMappingContext, 0, Options);
		}
	}

	Super::NotifyControllerChanged();
}
