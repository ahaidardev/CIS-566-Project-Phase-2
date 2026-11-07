// ABaseEnemy: LocomotionAndRotation. Included once by ABaseEnemy.cpp; retains original definition order.

// Called every frame
void AABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (DamageReactionComponent)
		DamageReactionComponent->UpdateKnockoutMotors(
			GameplayTagManagerComponent && GameplayTagManagerComponent->HasGameplayTag(TAG_Stealth_Enemy_KnockedOut));
	RefreshStealthLocomotion();
}

void AABaseEnemy::RefreshStealthLocomotion()
{
	auto* AI = Cast<AAIController>(GetController());
	const auto* Suspicion = FindComponentByClass<UModularSuspicionComponent>();
	auto* Movement = Cast<UAlsCharacterMovementComponent>(GetCharacterMovement());
	if (!AI || !Suspicion || !Movement || !PatrolMovementSettings || !InvestigationMovementSettings)
		return;
	if (GameplayTagManagerComponent && (GameplayTagManagerComponent->HasGameplayTag(TAG_Stealth_Enemy_Dead) ||
										GameplayTagManagerComponent->HasGameplayTag(TAG_Stealth_Enemy_KnockedOut)))
		return;
	const auto* CombatRole = FindComponentByClass<UEnemyCombatRoleComponent>();
	const auto* Brain = Cast<UBehaviorTreeComponent>(AI->GetBrainComponent());
	const auto& Trees = Suspicion->GetBehaviorTrees();
	const auto* Tree = Brain ? Brain->GetCurrentTree() : nullptr;
	const bool bCombat = (CombatRole && CombatRole->bCombatActive) || (Tree && Tree == Trees.AttackingTree);
	const bool bHidingSearch = HidingSearchComponent && HidingSearchComponent->bSearching;
	const bool bPatrolling = Tree && Tree == Trees.RoamingTree;
	const bool bInvestigating = !bCombat && !bPatrolling &&
								((Tree && (Tree == Trees.InvestigateTree || Tree == Trees.SearchingTree ||
										   Tree == Trees.SuspiciousTree || Tree == Trees.AlertedTree)) ||
								 Suspicion->GetSuspicion() > 0.0f);
	UAlsMovementSettings* DesiredSettings = bCombat			 ? MovementSettings.Get()
											: bInvestigating ? InvestigationMovementSettings.Get()
															 : PatrolMovementSettings.Get();
	// The attacking service or role chooses walking for positioning/strafe and jogging for longer moves.
	if (bCombat && GetDesiredGait() == AlsGaitTags::Walking)
		DesiredSettings = CombatPositioningMovementSettings.Get();
	if ((bCombat || bHidingSearch) && CombatRole && CombatRoleMovementSettings)
	{
		const float Speed = CombatRole->GetActiveMovementSpeed();
		const FGameplayTag RoleGait = CombatRole->GetActiveMovementGait();
		if (AppliedCombatRoleSpeed != Speed || AppliedCombatRoleGait != RoleGait)
		{
			for (auto& Rotation : CombatRoleMovementSettings->RotationModes)
				for (auto& StanceSettings : Rotation.Value.Stances)
				{
					auto& RoleGaitSettings = StanceSettings.Value;
					// The mode owns its speed; the chosen gait controls the ALS animation thresholds.
					RoleGaitSettings.bAllowDirectionDependentMovementSpeed = false;
					RoleGaitSettings.WalkForwardSpeed = RoleGaitSettings.WalkBackwardSpeed =
						RoleGait == AlsGaitTags::Walking ? Speed : Speed * 0.25f;
					RoleGaitSettings.RunForwardSpeed = RoleGaitSettings.RunBackwardSpeed =
						RoleGait == AlsGaitTags::Sprinting ? Speed * 0.5f : Speed;
					RoleGaitSettings.SprintSpeed = Speed;
				}
			AppliedCombatRoleSpeed = Speed;
			AppliedCombatRoleGait = RoleGait;
			// Refresh ALS's cached settings when Blueprint values or the movement mode change.
			if (AppliedMovementSettings == CombatRoleMovementSettings)
				AppliedMovementSettings = nullptr;
		}
		DesiredSettings = CombatRoleMovementSettings.Get();
	}
	FGameplayTag TreeDesiredGait = bInvestigating ? AlsGaitTags::Running : AlsGaitTags::Walking;
	// A BT movement task can override noncombat settings until its tree changes.
	EnemyMovementTask::Resolve(this, Tree, bCombat || bHidingSearch, DesiredSettings, TreeDesiredGait);
	if (AppliedMovementSettings != DesiredSettings)
	{
		Movement->SetMovementSettings(DesiredSettings);
		AppliedMovementSettings = DesiredSettings;
	}
	// Preserve the attacking service/role's close walk / long jog / safety sprint choice.
	if (bCombat || bHidingSearch)
		return;
	const auto* Blackboard = AI->GetBlackboardComponent();
	const auto* Squad = FindComponentByClass<USquadSuspicionComponent>();
	const bool bFollowingTracker =
		Blackboard && !Blackboard->GetValueAsBool(TEXT("Investigate")) && Squad && Squad->TrackerActor.IsValid() &&
		Squad->TrackerPhase == ETrackerSquadPhase::Accompanying &&
		(AI->GetMoveStatus() == EPathFollowingStatus::Moving || AI->GetMoveStatus() == EPathFollowingStatus::Waiting);
	// Roaming/return trees also contain investigation branches. Only route travel
	// releases ordinary focus; hearing memory and visible/recent targets still own facing.
	const bool bRouteTravel = Tree && (Tree == Trees.RoamingTree || Tree == Trees.ReturnToPostTree) && Blackboard &&
							  !Blackboard->GetValueAsBool(TEXT("Investigate"));
	const bool bTrackingHostile =
		Blackboard && Blackboard->GetValueAsObject(TEXT("Target")) &&
		(Blackboard->GetValueAsBool(TEXT("CanSeeTarget")) || Blackboard->GetValueAsBool(TEXT("RecentlySeenTarget")));
	const bool bHeardPlayerFocus =
		FAISystem::IsValidLocation(AI->GetFocalPointForPriority(EnemyFocusPriority::HeardPlayer));
	if (bHeardPlayerFocus || bTrackingHostile ||
		(!(bRouteTravel || bFollowingTracker) && FAISystem::IsValidLocation(AI->GetFocalPoint())))
	{
		// Restore focus-facing when perception/investigation takes over from a route.
		SetDesiredRotationMode(AlsRotationModeTags::ViewDirection);
	}
	else if (bRouteTravel || bFollowingTracker)
	{
		for (const auto Priority : {EAIFocusPriority::Default, EAIFocusPriority::Gameplay})
			if (FAISystem::IsValidLocation(AI->GetFocalPointForPriority(Priority)))
				AI->ClearFocus(Priority);
		SetDesiredAiming(false);
		SetDesiredRotationMode(AlsRotationModeTags::VelocityDirection);
	}
	SetDesiredGait(TreeDesiredGait);
}

bool AABaseEnemy::SmoothEnemyRotation(float DeltaTime, bool bMoving)
{
	const auto* AI = Cast<AAIController>(GetController());
	if (!AI || DeltaTime <= 0.0f)
		return false;
	if (GetRotationMode() != AlsRotationModeTags::VelocityDirection || !bMoving ||
		GetVelocity().SizeSquared2D() <= FMath::Square(10.0f))
		return false;
	const float TargetYaw = GetVelocity().Rotation().Yaw;
	SetRotationSmooth(TargetYaw, DeltaTime, FMath::Max(0.01f, TurnInterpolationHalfLife));
	return true;
}

bool AABaseEnemy::RefreshCustomGroundedMovingRotation(float DeltaTime)
{
	return SmoothEnemyRotation(DeltaTime, true);
}

bool AABaseEnemy::RefreshCustomGroundedNotMovingRotation(float DeltaTime)
{
	return SmoothEnemyRotation(DeltaTime, false);
}

// Called to bind functionality to input
void AABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
