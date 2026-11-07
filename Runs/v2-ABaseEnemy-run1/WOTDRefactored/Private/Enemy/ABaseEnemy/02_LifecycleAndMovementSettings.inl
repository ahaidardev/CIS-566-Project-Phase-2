// ABaseEnemy: LifecycleAndMovementSettings. Included once by ABaseEnemy.cpp; retains original definition order.

// Sets default values
AABaseEnemy::AABaseEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 90.0f, 90.0f);
	GameplayTagManagerComponent =
		CreateDefaultSubobject<UGameplayTagManagerComponent>(TEXT("GameplayTagManagerComponent"));
	CASComponent = CreateDefaultSubobject<UContextualAnimSceneActorComponent>(TEXT("CASComponent"));
	BarkComponent = CreateDefaultSubobject<UEnemyBarkComponent>(TEXT("BarkComponent"));
	HidingSearchComponent = CreateDefaultSubobject<UEnemyHidingSearchComponent>(TEXT("HidingSearchComponent"));
	DamageReactionComponent = CreateDefaultSubobject<UEnemyDamageReactionComponent>(TEXT("DamageReactionComponent"));
}

// Called when the game starts or when spawned
void AABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	Tags.AddUnique(TEXT("Enemy"));
	InitializeMovementSettings();
}

// Separate movement-settings construction from BeginPlay lifecycle orchestration.
void AABaseEnemy::InitializeMovementSettings()
{
	if (MovementSettings)
	{
		PatrolMovementSettings = DuplicateObject<UAlsMovementSettings>(MovementSettings, this);
		InvestigationMovementSettings = DuplicateObject<UAlsMovementSettings>(MovementSettings, this);
		for (auto& Rotation : PatrolMovementSettings->RotationModes)
			for (auto& StanceSettings : Rotation.Value.Stances)
				StanceSettings.Value.WalkForwardSpeed = StanceSettings.Value.WalkBackwardSpeed = PatrolWalkSpeed;
		for (auto& Rotation : InvestigationMovementSettings->RotationModes)
			for (auto& StanceSettings : Rotation.Value.Stances)
			{
				// Keep 100 cm/s above the walking threshold so ALS uses its running gait.
				StanceSettings.Value.WalkForwardSpeed = StanceSettings.Value.WalkBackwardSpeed =
					InvestigationJogSpeed * 0.25f;
				StanceSettings.Value.RunForwardSpeed = StanceSettings.Value.RunBackwardSpeed = InvestigationJogSpeed;
			}
		// Combat gets its own settings; shared ALS assets and noncombat speeds stay intact.
		MovementSettings = DuplicateObject<UAlsMovementSettings>(MovementSettings, this);
		for (auto& Rotation : MovementSettings->RotationModes)
			for (auto& StanceSettings : Rotation.Value.Stances)
			{
				auto& CombatGaitSettings = StanceSettings.Value;
				// Keep the walking threshold below jogging so ALS selects its running gait.
				CombatGaitSettings.WalkForwardSpeed = CombatGaitSettings.WalkBackwardSpeed = AttackJogSpeed * 0.25f;
				CombatGaitSettings.RunForwardSpeed = CombatGaitSettings.RunBackwardSpeed = AttackJogSpeed;
				CombatGaitSettings.SprintSpeed = FMath::Max(CombatGaitSettings.SprintSpeed, AttackJogSpeed);
			}
		CombatPositioningMovementSettings = DuplicateObject<UAlsMovementSettings>(MovementSettings, this);
		CombatRoleMovementSettings = DuplicateObject<UAlsMovementSettings>(MovementSettings, this);
		for (auto& Rotation : CombatPositioningMovementSettings->RotationModes)
			for (auto& StanceSettings : Rotation.Value.Stances)
			{
				auto& CombatGaitSettings = StanceSettings.Value;
				CombatGaitSettings.bAllowDirectionDependentMovementSpeed = false;
				CombatGaitSettings.WalkForwardSpeed = CombatGaitSettings.WalkBackwardSpeed = AttackWalkForwardSpeed;
				CombatGaitSettings.RunForwardSpeed =
					FMath::Max(CombatGaitSettings.RunForwardSpeed, AttackWalkForwardSpeed);
				CombatGaitSettings.RunBackwardSpeed =
					FMath::Max(CombatGaitSettings.RunBackwardSpeed, AttackWalkForwardSpeed);
				CombatGaitSettings.SprintSpeed = FMath::Max(CombatGaitSettings.SprintSpeed, AttackWalkForwardSpeed);
			}
	}
}
