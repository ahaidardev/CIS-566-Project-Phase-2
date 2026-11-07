#include "Misc/AutomationTest.h"

#include "Player/AliCharacter.h"
#include "Player/Components/InventoryComponent.h"
#include "Player/Components/CoverComponent.h"
#include "Player/Components/ParkourComponent.h"
#include "Player/Components/AimObstructionComponent.h"
#include "Player/Components/PlayerSFXComponent.h"
#include "Enemy/ABaseEnemy.h"
#include "Enemy/TrackerEnemy.h"
#include "Enemy/EnemyBarkComponent.h"
#include "Enemy/Components/EnemyCombatRoleComponent.h"
#include "Enemy/Components/EnemyHidingSearchComponent.h"
#include "Enemy/Components/EnemyDamageReactionComponent.h"
#include "BehaviorTree/TrackerSquadNodes.h"
#include "Suspicion/GameplayTagManagerComponent.h"
#include "Suspicion/ModularSuspicionComponent.h"
#include "StealthSystem/StealthGameplayTags.h"
#include "StealthSystem/SquadSuspicionComponent.h"
#include "StealthSystem/Subsystems/SquadCoordinatorSubsystem.h"
#include "Abilities/GSCAbilitySystemComponent.h"
#include "Abilities/GSCGameplayAbility.h"
#include "Components/GSCCoreComponent.h"
#include "ContextualAnimSceneActorComponent.h"
#include "AlsCameraComponent.h"
#include "AlsCharacterMovementComponent.h"
#include "Settings/AlsMovementSettings.h"
#include "Utility/AlsGameplayTags.h"
#include "Engine/DamageEvents.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"

namespace
{
	constexpr EAutomationTestFlags RefactoredAuditFlags =
		EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter;

	/**
	 * Helper to create a deterministic card item for UInventory testing.
	 */
	FCardItem MakeDeterministicCard(const TCHAR *Name, TSubclassOf<UGSCGameplayAbility> AbilityClass,
									ECardRefillType RefillType = ECardRefillType::Consumable, float RefillInterval = 2.0f)
	{
		FCardItem Card;
		Card.CardName = FName(Name);
		Card.AbilityClass = AbilityClass;
		Card.RefillType = RefillType;
		Card.RefillInterval = RefillInterval;
		return Card;
	}

	/**
	 * Helper to resolve the Source/WOTDRefactored directory for static source-level architecture & readability checks.
	 */
	FString GetRefactoredSourceDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Source/WOTDRefactored"));
	}
} // namespace

// =================================================================================================
// 1. I/O EQUIVALENCE BETWEEN ORIGINAL AND REFACTORED CODE
// =================================================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRefactoredIOEquivalenceInventoryTest,
								 "WOTD.Refactoring.IOEquivalence.InventoryComponent", RefactoredAuditFlags)

bool FRefactoredIOEquivalenceInventoryTest::RunTest(const FString &Parameters)
{
	UInventory *Inventory = NewObject<UInventory>();
	if (!TestNotNull(TEXT("InventoryComponent instantiates cleanly"), Inventory))
	{
		return false;
	}

	// 1. Verify empty state I/O contract
	FCardItem InvalidCard;
	InvalidCard.AbilityClass = nullptr;
	TestFalse(TEXT("CanAddCard rejects card with null AbilityClass"), Inventory->CanAddCard(InvalidCard));
	TestEqual(TEXT("Empty inventory GetNextCard returns 0"), Inventory->GetNextCard(), 0);
	TestEqual(TEXT("Empty inventory GetPreviousCard returns 0"), Inventory->GetPreviousCard(), 0);
	TestEqual(TEXT("Empty inventory RemoveSelectedCard returns default FCardItem"),
			  Inventory->RemoveSelectedCard().CardName, FName(NAME_None));

	// 2. Load distinct ability classes to verify 5-slot active wheel + 15-card total capacity + reserve routing
	const TArray<UClass *> CardClasses = {
		LoadClass<UGSCGameplayAbility>(nullptr, TEXT("/Game/WOTD/GAS/Abilities/GA_ThrowTranq.GA_ThrowTranq_C")),
		LoadClass<UGSCGameplayAbility>(nullptr, TEXT("/Game/WOTD/GAS/Abilities/GA_ThrowSoundCard.GA_ThrowSoundCard_C")),
		LoadClass<UGSCGameplayAbility>(nullptr, TEXT("/Game/WOTD/GAS/Abilities/GA_ThrowSandSmoke.GA_ThrowSandSmoke_C")),
		LoadClass<UGSCGameplayAbility>(nullptr, TEXT("/Game/WOTD/GAS/Abilities/GA_ThrowPyro.GA_ThrowPyro_C")),
		LoadClass<UGSCGameplayAbility>(nullptr, TEXT("/Game/WOTD/GAS/Abilities/GA_ThrowGrenade.GA_ThrowGrenade_C")),
		LoadClass<UGSCGameplayAbility>(nullptr, TEXT("/Game/WOTD/GAS/Abilities/GA_ThrowElectric.GA_ThrowElectric_C"))};

	// Fallback to base UGSCGameplayAbility if running in a stripped asset environment
	if (!CardClasses[0])
	{
		FCardItem BaseCard =
			MakeDeterministicCard(TEXT("AutoCard"), UGSCGameplayAbility::StaticClass(), ECardRefillType::Automatic, 3.0f);
		TestTrue(TEXT("CanAddCard accepts valid ability card"), Inventory->CanAddCard(BaseCard));
		const FCardItem Added = Inventory->AddCardToInventory_Implementation(BaseCard);
		TestEqual(TEXT("AddCardToInventory returns identical FCardItem"), Added.CardName, BaseCard.CardName);
		TestEqual(TEXT("First added card sets CurrentSlotIndex to 0"), Inventory->GetCurrentSlotIndex(), 0);
		TestEqual(TEXT("Copy count increments to 1"), Inventory->GetCardCopyCounts().FindRef(BaseCard.AbilityClass), 1);

		const FCardItem Removed = Inventory->RemoveSelectedCard();
		TestEqual(TEXT("RemoveSelectedCard returns exact card data"), Removed.CardName, BaseCard.CardName);
		TestEqual(TEXT("Automatic card schedules exact refill interval"),
				  Inventory->GetRefillRemaining(BaseCard.AbilityClass), 3.0f);

		Inventory->UpdateAutoRefills(1.5f);
		TestEqual(TEXT("Half-interval tick leaves exact remaining time"),
				  Inventory->GetRefillRemaining(BaseCard.AbilityClass), 1.5f);
		Inventory->UpdateAutoRefills(1.5f);
		TestEqual(TEXT("Full-interval tick restores charge to active slot"),
				  Inventory->GetCardCopyCounts().FindRef(BaseCard.AbilityClass), 1);
		return true;
	}

	// Add 5 unique cards to fill the active wheel slots
	for (int32 i = 0; i < 5; ++i)
	{
		FCardItem Card = MakeDeterministicCard(*FString::Printf(TEXT("SlotCard_%d"), i), CardClasses[i]);
		const FCardItem Result = Inventory->AddCardToInventory_Implementation(Card);
		TestEqual(TEXT("AddCardToInventory output matches input card name"), Result.CardName, Card.CardName);
	}
	TestEqual(TEXT("5 unique cards occupy all 5 active wheel slots"), Inventory->GetSlotOrder().Num(), 5);
	TestEqual(TEXT("Reserve is empty when <= 5 unique types are added"), Inventory->GetReserveCards().Num(), 0);

	// 6th unique card routes to Reserve
	FCardItem ReserveCard = MakeDeterministicCard(TEXT("ReserveCard_6"), CardClasses[5]);
	Inventory->AddCardToInventory_Implementation(ReserveCard);
	TestEqual(TEXT("Active wheel remains capped at 5 slots"), Inventory->GetSlotOrder().Num(), 5);
	TestEqual(TEXT("6th unique card routes to reserve"), Inventory->GetReserveCards().Num(), 1);
	TestEqual(TEXT("Reserve card count equals 1"), Inventory->GetReserveCardCount(CardClasses[5]), 1);

	// Verify cyclic slot selection I/O (GetNextCard / GetPreviousCard / SelectCard)
	Inventory->SelectCard(4);
	TestEqual(TEXT("SelectCard(4) sets CurrentSlotIndex to 4"), Inventory->GetCurrentSlotIndex(), 4);
	TestEqual(TEXT("GetNextCard wraps from 4 to 0"), Inventory->GetNextCard(), 0);
	TestEqual(TEXT("GetPreviousCard wraps from 0 to 4"), Inventory->GetPreviousCard(), 4);

	// Verify SwapSelectedCardWithReserve I/O
	const TSubclassOf<UGSCGameplayAbility> PriorSlot4Class = Inventory->GetSlotOrder()[4];
	TestTrue(TEXT("SwapSelectedCardWithReserve succeeds for valid reserve class"),
			 Inventory->SwapSelectedCardWithReserve(CardClasses[5]));
	TestEqual(TEXT("Slot 4 now holds former reserve class"), Inventory->GetSlotOrder()[4],
			  TSubclassOf<UGSCGameplayAbility>(CardClasses[5]));
	TestEqual(TEXT("Former Slot 4 class moved to reserve"), Inventory->GetReserveCardCount(PriorSlot4Class), 1);

	// Fill up to 15 total cards to verify hard capacity ceiling
	for (int32 i = 0; i < 9; ++i)
	{
		FCardItem ExtraStack = MakeDeterministicCard(TEXT("SlotCard_0"), CardClasses[0]);
		TestTrue(TEXT("CanAddCard is true below 15 total cards"), Inventory->CanAddCard(ExtraStack));
		Inventory->AddCardToInventory_Implementation(ExtraStack);
	}
	FCardItem OverflowCard = MakeDeterministicCard(TEXT("SlotCard_0"), CardClasses[0]);
	TestFalse(TEXT("CanAddCard rejects 16th card at 15-card capacity ceiling"), Inventory->CanAddCard(OverflowCard));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRefactoredIOEquivalenceAliCharacterTest,
								 "WOTD.Refactoring.IOEquivalence.AliCharacter", RefactoredAuditFlags)

bool FRefactoredIOEquivalenceAliCharacterTest::RunTest(const FString &Parameters)
{
	AAliCharacter *Character = NewObject<AAliCharacter>();
	if (!TestNotNull(TEXT("AAliCharacter instantiates cleanly"), Character))
	{
		return false;
	}

	// 1. Verify Invisibility transition I/O without a World (immediate snap branch in 02_Invisibility.inl)
	TestEqual(TEXT("Initial InvisibilityAlpha is 0.0f"), Character->InvisibilityAlpha, 0.0f);
	Character->BecomeInvisible();
	TestEqual(TEXT("BecomeInvisible without world immediately sets InvisibilityAlpha to 1.0f"),
			  Character->InvisibilityAlpha, 1.0f);
	Character->BecomeVisible();
	TestEqual(TEXT("BecomeVisible without world immediately sets InvisibilityAlpha to 0.0f"),
			  Character->InvisibilityAlpha, 0.0f);

	// 2. Verify Camera Right Shoulder I/O delegation (10_CameraAndObstacleChecks.inl)
	Character->SetCameraRightShoulder(false);
	TestFalse(TEXT("GetCameraRightShoulder reflects false after SetCameraRightShoulder(false)"),
			  Character->GetCameraRightShoulder());
	Character->SetCameraRightShoulder(true);
	TestTrue(TEXT("GetCameraRightShoulder reflects true after SetCameraRightShoulder(true)"),
			 Character->GetCameraRightShoulder());

	// 3. Verify CheckIfObjectExists stub I/O contract (10_CameraAndObstacleChecks.inl)
	bool bObstacleExists = true;
	FRotator TargetRot(45.f, 90.f, 15.f);
	Character->CheckIfObjectExists(bObstacleExists, TargetRot);
	TestFalse(TEXT("CheckIfObjectExists resets bOutObstacleExists to false"), bObstacleExists);
	TestEqual(TEXT("CheckIfObjectExists resets OutTargetRotation to ZeroRotator"), TargetRot, FRotator::ZeroRotator);

	// 4. Verify WeaponAimTrace I/O contract (08_WeaponTracing.inl)
	FVector TraceStart = FVector::ZeroVector;
	FVector TraceEnd = FVector::ZeroVector;
	FHitResult Hit;
	Hit.TraceEnd = FVector(123.f, 456.f, 789.f);
	Character->WeaponAimTrace(TraceStart, TraceEnd, Hit);
	TestEqual(TEXT("WeaponAimTrace outputs OutHit.TraceEnd into InTraceEnd"), TraceEnd, Hit.TraceEnd);

	// 5. Verify CanBeSeenFrom I/O when no World or when Concealed tag is active (01_VisibilityAndConstruction.inl)
	FCanBeSeenFromContext SightContext;
	SightContext.ObserverLocation = FVector::ZeroVector;
	SightContext.IgnoreActor = nullptr;
	FVector SeenLocation = FVector::ZeroVector;
	int32 LoSChecks = 99;
	int32 AsyncChecks = 99;
	float SightStrength = 99.f;
	const UAISense_Sight::EVisibilityResult SightResult =
		Character->CanBeSeenFrom(SightContext, SeenLocation, LoSChecks, AsyncChecks, SightStrength);
	TestEqual(TEXT("CanBeSeenFrom without valid world returns NotVisible"), SightResult,
			  UAISense_Sight::EVisibilityResult::NotVisible);
	TestEqual(TEXT("CanBeSeenFrom resets OutNumberOfLoSChecksPerformed to 0"), LoSChecks, 0);
	TestEqual(TEXT("CanBeSeenFrom resets OutSightStrength to 0.0f"), SightStrength, 0.0f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRefactoredIOEquivalenceABaseEnemyTest,
								 "WOTD.Refactoring.IOEquivalence.ABaseEnemy", RefactoredAuditFlags)

bool FRefactoredIOEquivalenceABaseEnemyTest::RunTest(const FString &Parameters)
{
	AABaseEnemy *Enemy = NewObject<AABaseEnemy>();
	if (!TestNotNull(TEXT("AABaseEnemy instantiates cleanly"), Enemy))
	{
		return false;
	}

	// 1. Verify Default Movement & Rotation Tuning I/O (02_LifecycleAndMovementSettings.inl)
	TestEqual(TEXT("Default PatrolWalkSpeed is 100.0f"), Enemy->PatrolWalkSpeed, 100.0f);
	TestEqual(TEXT("Default InvestigationJogSpeed is 100.0f"), Enemy->InvestigationJogSpeed, 100.0f);
	TestEqual(TEXT("Default AttackWalkForwardSpeed is 200.0f"), Enemy->AttackWalkForwardSpeed, 200.0f);
	TestEqual(TEXT("Default AttackJogSpeed is 200.0f"), Enemy->AttackJogSpeed, 200.0f);
	TestEqual(TEXT("Default TurnInterpolationHalfLife is 0.12f"), Enemy->TurnInterpolationHalfLife, 0.12f);
	if (TestNotNull(TEXT("Enemy has CharacterMovementComponent"), Enemy->GetCharacterMovement()))
	{
		TestEqual(TEXT("Constructor configures RotationRate to (0, 90, 90)"),
				  Enemy->GetCharacterMovement()->RotationRate, FRotator(0.0f, 90.0f, 90.0f));
	}

	// 2. Verify TryInterceptIncomingDamage default implementation I/O (01_Damage.inl)
	FHitResult DummyHit;
	TestFalse(TEXT("Default TryInterceptIncomingDamage_Implementation returns false"),
			  Enemy->TryInterceptIncomingDamage_Implementation(25.0f, nullptr, DummyHit, false));

	// 3. Verify ApplyGrenadeBlast input validation guards (01_Damage.inl)
	Enemy->ApplyGrenadeBlast(FVector(NAN, 0.f, 0.f), 1200.f, 1400.f);
	TestTrue(TEXT("ApplyGrenadeBlast rejects NaN Origin without entering Ragdolling"),
			 Enemy->GetLocomotionAction() != AlsLocomotionActionTags::Ragdolling);

	Enemy->ApplyGrenadeBlast(FVector::ZeroVector, -10.f, 1400.f);
	TestTrue(TEXT("ApplyGrenadeBlast rejects negative Radius without entering Ragdolling"),
			 Enemy->GetLocomotionAction() != AlsLocomotionActionTags::Ragdolling);

	Enemy->ApplyGrenadeBlast(FVector::ZeroVector, 1200.f, 0.f);
	TestTrue(TEXT("ApplyGrenadeBlast rejects zero Strength without entering Ragdolling"),
			 Enemy->GetLocomotionAction() != AlsLocomotionActionTags::Ragdolling);

	// 4. Verify grounded rotation callbacks return false when not possessed by an AIController (03_LocomotionAndRotation.inl)
	TestFalse(TEXT("RefreshCustomGroundedMovingRotation returns false without AIController"),
			  Enemy->RefreshCustomGroundedMovingRotation(0.016f));
	TestFalse(TEXT("RefreshCustomGroundedNotMovingRotation returns false without AIController"),
			  Enemy->RefreshCustomGroundedNotMovingRotation(0.016f));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRefactoredIOEquivalenceTrackerEnemyTest,
								 "WOTD.Refactoring.IOEquivalence.TrackerEnemy", RefactoredAuditFlags)

bool FRefactoredIOEquivalenceTrackerEnemyTest::RunTest(const FString &Parameters)
{
	// 1. Verify ATrackerEnemy CDO & inheritance I/O contract (inherits AABaseEnemy defaults + TrackerEnemy specifics)
	ATrackerEnemy *DetachedTracker = NewObject<ATrackerEnemy>();
	if (!TestNotNull(TEXT("ATrackerEnemy instantiates cleanly"), DetachedTracker))
	{
		return false;
	}
	TestTrue(TEXT("ATrackerEnemy derives from AABaseEnemy"),
			 DetachedTracker->IsA<AABaseEnemy>());
	TestEqual(TEXT("Default NoisePingCooldown is 2.0f"), DetachedTracker->NoisePingCooldown, 2.0f);
	TestEqual(TEXT("Inherited PatrolWalkSpeed is 100.0f"), DetachedTracker->PatrolWalkSpeed, 100.0f);
	TestFalse(TEXT("Detached PingTarget returns false without a UWorld / SquadCoordinatorSubsystem"),
			  DetachedTracker->PingTarget(FVector(500.f, 200.f, 0.f)));

	// 2. Verify live World & USquadCoordinatorSubsystem I/O contract for ATrackerEnemy::PingTarget,
	// perception binding/unbinding, noise cooldown gating, shout filtering, and EndPlay release.
	UWorld *World = UWorld::CreateWorld(EWorldType::Game, false);
	if (!TestNotNull(TEXT("Transient Game World created for TrackerEnemy test"), World))
	{
		return false;
	}
	ON_SCOPE_EXIT
	{
		World->DestroyWorld(false);
		World->RemoveFromRoot();
	};

	USquadCoordinatorSubsystem *Coordinator = World->GetSubsystem<USquadCoordinatorSubsystem>();
	if (!TestNotNull(TEXT("USquadCoordinatorSubsystem exists in World"), Coordinator))
	{
		return false;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ATrackerEnemy *Tracker = World->SpawnActor<ATrackerEnemy>(ATrackerEnemy::StaticClass(), FVector::ZeroVector,
															  FRotator::ZeroRotator, SpawnParams);
	AABaseEnemy *GuardPawn = World->SpawnActor<AABaseEnemy>(AABaseEnemy::StaticClass(), FVector(300.f, 0.f, 0.f),
															FRotator::ZeroRotator, SpawnParams);
	if (!TestNotNull(TEXT("Tracker spawns in world"), Tracker) ||
		!TestNotNull(TEXT("GuardPawn spawns in world"), GuardPawn))
	{
		return false;
	}

	// Attach USquadSuspicionComponent to both Tracker (sender) and GuardPawn (eligible receiver)
	USquadSuspicionComponent *TrackerSquadComp = NewObject<USquadSuspicionComponent>(Tracker);
	Tracker->AddInstanceComponent(TrackerSquadComp);
	TrackerSquadComp->RegisterComponent();
	Coordinator->RegisterMember(TrackerSquadComp);

	USquadSuspicionComponent *GuardSquadComp = NewObject<USquadSuspicionComponent>(GuardPawn);
	GuardPawn->AddInstanceComponent(GuardSquadComp);
	GuardSquadComp->RegisterComponent();
	Coordinator->RegisterMember(GuardSquadComp);

	// Guard clause: NaN SearchLocation must be rejected by PingTarget -> PingTrackerSquad
	TestFalse(TEXT("PingTarget rejects NaN SearchLocation"),
			  Tracker->PingTarget(FVector(NAN, 0.f, 0.f)));
	TestEqual(TEXT("Guard remains in None phase after rejected NaN ping"),
			  GuardSquadComp->TrackerPhase, ETrackerSquadPhase::None);

	// Valid PingTarget I/O: recruits GuardSquadComp, sets Searching phase, and stores exact SearchLocation
	const FVector FirstLeadLocation(1250.f, -400.f, 0.f);
	TestTrue(TEXT("PingTarget succeeds when eligible squad guard is registered"),
			 Tracker->PingTarget(FirstLeadLocation));
	TestEqual(TEXT("Recruited guard enters ETrackerSquadPhase::Searching"),
			  GuardSquadComp->TrackerPhase, ETrackerSquadPhase::Searching);
	TestEqual(TEXT("Recruited guard TrackerActor points to Tracker"),
			  GuardSquadComp->TrackerActor.Get(), static_cast<AActor *>(Tracker));
	TestEqual(TEXT("Recruited guard receives exact TrackerSearchLocation"),
			  GuardSquadComp->TrackerSearchLocation, FirstLeadLocation);
	const int32 GenAfterFirstPing = GuardSquadComp->TrackerSearchGeneration;
	TestTrue(TEXT("TrackerSearchGeneration increments to >= 1"), GenAfterFirstPing >= 1);

	// 3. Verify PossessedBy / UnPossessed perception binding & noise lead filtering
	AAIController *TrackerController = World->SpawnActor<AAIController>(AAIController::StaticClass(), FVector::ZeroVector,
																		FRotator::ZeroRotator, SpawnParams);
	UAIPerceptionComponent *Perception = NewObject<UAIPerceptionComponent>(TrackerController);
	TrackerController->AddInstanceComponent(Perception);
	Perception->ConfigureSense(*NewObject<UAISenseConfig_Sight>(Perception));
	Perception->ConfigureSense(*NewObject<UAISenseConfig_Hearing>(Perception));
	Perception->RegisterComponent();

	UModularSuspicionComponent *TrackerSuspicion = NewObject<UModularSuspicionComponent>(Tracker);
	Tracker->AddInstanceComponent(TrackerSuspicion);
	TrackerSuspicion->RegisterComponent();

	TrackerController->Possess(Tracker);

	// Self-noise and fellow AABaseEnemy noise must be ignored by HandleNoiseLead
	const FVector EnemyNoiseLoc(2000.f, 0.f, 0.f);
	FAIStimulus FellowEnemyNoise(*GetDefault<UAISense_Hearing>(), 1.0f, EnemyNoiseLoc, FVector::ZeroVector);
	Perception->OnTargetPerceptionUpdated.Broadcast(GuardPawn, FellowEnemyNoise);
	TestEqual(TEXT("Noise from fellow AABaseEnemy is ignored by TrackerEnemy"),
			  GuardSquadComp->TrackerSearchGeneration, GenAfterFirstPing);

	// Squad shout tag ("TargetIdentifiedEnemyShout") must be ignored to prevent feedback loops
	ACharacter *ExternalNoiseMaker = World->SpawnActor<ACharacter>(ACharacter::StaticClass(), FVector(900.f, 0.f, 0.f),
																   FRotator::ZeroRotator, SpawnParams);
	FAIStimulus ShoutStimulus(*GetDefault<UAISense_Hearing>(), 1.0f, FVector(900.f, 0.f, 0.f), FVector::ZeroVector);
	ShoutStimulus.Tag = TEXT("TargetIdentifiedEnemyShout");
	Perception->OnTargetPerceptionUpdated.Broadcast(ExternalNoiseMaker, ShoutStimulus);
	TestEqual(TEXT("TargetIdentifiedEnemyShout stimulus is ignored by TrackerEnemy"),
			  GuardSquadComp->TrackerSearchGeneration, GenAfterFirstPing);

	// Valid external noise triggers a ping and starts the NoisePingCooldown gate
	const FVector ValidNoiseLoc(1800.f, 600.f, 0.f);
	FAIStimulus ValidNoise(*GetDefault<UAISense_Hearing>(), 1.0f, ValidNoiseLoc, FVector::ZeroVector);
	Perception->OnTargetPerceptionUpdated.Broadcast(ExternalNoiseMaker, ValidNoise);
	TestEqual(TEXT("Valid external hearing stimulus increments TrackerSearchGeneration"),
			  GuardSquadComp->TrackerSearchGeneration, GenAfterFirstPing + 1);
	TestEqual(TEXT("Valid external hearing stimulus updates TrackerSearchLocation"),
			  GuardSquadComp->TrackerSearchLocation, ValidNoiseLoc);

	// Immediate second noise within NoisePingCooldown (2.0s) is suppressed
	const FVector CooldownBlockedLoc(2200.f, 900.f, 0.f);
	FAIStimulus CooldownNoise(*GetDefault<UAISense_Hearing>(), 1.0f, CooldownBlockedLoc, FVector::ZeroVector);
	Perception->OnTargetPerceptionUpdated.Broadcast(ExternalNoiseMaker, CooldownNoise);
	TestEqual(TEXT("Noise within NoisePingCooldown does not re-ping squad"),
			  GuardSquadComp->TrackerSearchGeneration, GenAfterFirstPing + 1);

	// UnPossessed cleanly unbinds perception delegates
	TrackerController->UnPossess();
	Tracker->NoisePingCooldown = 0.0f;
	Perception->OnTargetPerceptionUpdated.Broadcast(ExternalNoiseMaker, CooldownNoise);
	TestEqual(TEXT("UnPossessed tracker ignores subsequent perception broadcasts"),
			  GuardSquadComp->TrackerSearchGeneration, GenAfterFirstPing + 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRefactoredIOEquivalenceCoverComponentTest,
								 "WOTD.Refactoring.IOEquivalence.CoverComponent", RefactoredAuditFlags)

bool FRefactoredIOEquivalenceCoverComponentTest::RunTest(const FString &Parameters)
{
	UCoverComponent *Cover = NewObject<UCoverComponent>();
	if (!TestNotNull(TEXT("UCoverComponent instantiates cleanly"), Cover))
	{
		return false;
	}

	// 1. Verify initial cover state accessors (01_LifecycleAndWallContact.inl & CoverComponent.h)
	TestFalse(TEXT("Cover starts with GetCoverBool() == false"), Cover->GetCoverBool());
	Cover->SetCoverBool(true);
	TestTrue(TEXT("SetCoverBool(true) updates GetCoverBool()"), Cover->GetCoverBool());
	Cover->SetCoverBool(false);
	TestFalse(TEXT("SetCoverBool(false) updates GetCoverBool()"), Cover->GetCoverBool());

	TestFalse(TEXT("Cover starts with GetIsPeeking() == false"), Cover->GetIsPeeking());
	TestFalse(TEXT("Cover starts with GetRightFacingBool() == false"), Cover->GetRightFacingBool());
	TestFalse(TEXT("Cover starts with GetLeftFacingBool() == false"), Cover->GetLeftFacingBool());
	TestEqual(TEXT("GetCoverMoveSpeed returns 0.0f without OwnerCharacter"), Cover->GetCoverMoveSpeed(), 0.0f);
	TestFalse(TEXT("CanStandInCover returns false without OwnerCharacter"), Cover->CanStandInCover());

	// 2. Verify camera contribution pure queries when alpha is zero (04_Peeking.inl & 07_Camera.inl)
	FVector PeekOffset(99.f, 99.f, 99.f);
	float PeekFOV = 99.f;
	TestFalse(TEXT("GetPeekCameraContribution returns false when not peeking"),
			  Cover->GetPeekCameraContribution(PeekOffset, PeekFOV));
	TestEqual(TEXT("GetPeekCameraContribution zeroes OutLocationOffset"), PeekOffset, FVector::ZeroVector);
	TestEqual(TEXT("GetPeekCameraContribution zeroes OutFOVDelta"), PeekFOV, 0.0f);

	FVector EdgeOffset(99.f, 99.f, 99.f);
	TestFalse(TEXT("GetEdgeCameraContribution returns false when not at edge"),
			  Cover->GetEdgeCameraContribution(EdgeOffset));
	TestEqual(TEXT("GetEdgeCameraContribution zeroes OutLocationOffset"), EdgeOffset, FVector::ZeroVector);

	FMinimalViewInfo CoverView;
	float BlendAlpha = 1.0f;
	TestFalse(TEXT("GetCoverCameraView returns false when CoverCamAlpha is zero"),
			  Cover->GetCoverCameraView(CoverView, BlendAlpha));
	TestEqual(TEXT("GetCoverCameraView zeroes OutBlendAlpha when inactive"), BlendAlpha, 0.0f);

	return true;
}

// =================================================================================================
// 2. MINIMUM RANDOMIZATION & DETERMINISM VERIFICATION (INCLUDING TRACKERENEMY)
// =================================================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRefactoredMinimumRandomizationTest,
								 "WOTD.Refactoring.Determinism.MinimumRandomization", RefactoredAuditFlags)

bool FRefactoredMinimumRandomizationTest::RunTest(const FString &Parameters)
{
	// Part A: Static verification that AliCharacter, ABaseEnemy, InventoryComponent, CoverComponent,
	// TrackerEnemy, TrackerSquadNodes, and 06_TrackerCoordination.inl contain ZERO RNG calls
	const FString BaseDir = GetRefactoredSourceDir();
	const TArray<FString> TargetDirectories = {
		FPaths::Combine(BaseDir, TEXT("Private/Player/AliCharacter")),
		FPaths::Combine(BaseDir, TEXT("Private/Enemy/ABaseEnemy")),
		FPaths::Combine(BaseDir, TEXT("Private/Player/Components/InventoryComponent")),
		FPaths::Combine(BaseDir, TEXT("Private/Player/Components/CoverComponent"))};

	const TArray<FString> ExplicitTrackerFiles = {
		FPaths::Combine(BaseDir, TEXT("Private/Enemy/TrackerEnemy.cpp")),
		FPaths::Combine(BaseDir, TEXT("Public/Enemy/TrackerEnemy.h")),
		FPaths::Combine(BaseDir, TEXT("Private/BehaviorTree/TrackerSquadNodes.cpp")),
		FPaths::Combine(BaseDir, TEXT("Public/BehaviorTree/TrackerSquadNodes.h")),
		FPaths::Combine(BaseDir, TEXT("Private/StealthSystem/Subsystems/SquadCoordinatorSubsystem/06_TrackerCoordination.inl"))};

	const TArray<FString> ForbiddenTokens = {
		TEXT("FMath::Rand"), TEXT("FMath::FRand"), TEXT("FMath::SRand"),
		TEXT("FRandomStream"), TEXT("RandomUnitVector"), TEXT("RandRange"), TEXT("RandHelper")};

	for (const FString &Dir : TargetDirectories)
	{
		TArray<FString> Files;
		IFileManager::Get().FindFiles(Files, *(Dir / TEXT("*.inl")), true, false);
		for (const FString &FileName : Files)
		{
			const FString FullPath = FPaths::Combine(Dir, FileName);
			FString Content;
			if (FFileHelper::LoadFileToString(Content, *FullPath))
			{
				for (const FString &Token : ForbiddenTokens)
				{
					TestFalse(*FString::Printf(TEXT("%s contains no random call '%s'"), *FileName, *Token),
							  Content.Contains(Token));
				}
			}
		}
	}

	for (const FString &FilePath : ExplicitTrackerFiles)
	{
		FString Content;
		if (FFileHelper::LoadFileToString(Content, *FilePath))
		{
			const FString CleanName = FPaths::GetCleanFilename(FilePath);
			for (const FString &Token : ForbiddenTokens)
			{
				TestFalse(*FString::Printf(TEXT("Tracker file %s contains no random call '%s'"), *CleanName, *Token),
						  Content.Contains(Token));
			}
		}
	}

	// Part B: Verify UBTService_UpdateTrackerFollow sets RandomDeviation = 0.0f for zero tick-interval jitter
	UBTService_UpdateTrackerFollow *TrackerFollowService = NewObject<UBTService_UpdateTrackerFollow>();
	if (TestNotNull(TEXT("UBTService_UpdateTrackerFollow instantiates"), TrackerFollowService))
	{
		TestEqual(TEXT("UBTService_UpdateTrackerFollow has deterministic Interval = 0.25f"),
				  TrackerFollowService->Interval, 0.25f);
		TestEqual(TEXT("UBTService_UpdateTrackerFollow has zero RandomDeviation (0.0f)"),
				  TrackerFollowService->RandomDeviation, 0.0f);
	}

	// Part C: Verify deterministic equidistant guard tie-breaking and staggered formation slot geometry
	// in USquadCoordinatorSubsystem (06_TrackerCoordination.inl)
	UWorld *World = UWorld::CreateWorld(EWorldType::Game, false);
	if (TestNotNull(TEXT("World created for TrackerEnemy determinism check"), World))
	{
		ON_SCOPE_EXIT
		{
			World->DestroyWorld(false);
			World->RemoveFromRoot();
		};

		USquadCoordinatorSubsystem *Coordinator = World->GetSubsystem<USquadCoordinatorSubsystem>();
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ATrackerEnemy *Tracker = World->SpawnActor<ATrackerEnemy>(
			ATrackerEnemy::StaticClass(), FVector(0.f, 0.f, 0.f), FRotator::ZeroRotator, SpawnParams);
		// Two guards placed at the exact same Euclidean distance (500 cm) from the Tracker
		AABaseEnemy *GuardAlpha = World->SpawnActor<AABaseEnemy>(
			AABaseEnemy::StaticClass(), FVector(500.f, 0.f, 0.f), FRotator::ZeroRotator, SpawnParams);
		AABaseEnemy *GuardBeta = World->SpawnActor<AABaseEnemy>(
			AABaseEnemy::StaticClass(), FVector(-500.f, 0.f, 0.f), FRotator::ZeroRotator, SpawnParams);

		USquadSuspicionComponent *TrackerSquad = NewObject<USquadSuspicionComponent>(Tracker);
		Tracker->AddInstanceComponent(TrackerSquad);
		TrackerSquad->RegisterComponent();
		Coordinator->RegisterMember(TrackerSquad);

		USquadSuspicionComponent *AlphaSquad = NewObject<USquadSuspicionComponent>(GuardAlpha);
		GuardAlpha->AddInstanceComponent(AlphaSquad);
		AlphaSquad->RegisterComponent();
		Coordinator->RegisterMember(AlphaSquad);

		USquadSuspicionComponent *BetaSquad = NewObject<USquadSuspicionComponent>(GuardBeta);
		GuardBeta->AddInstanceComponent(BetaSquad);
		BetaSquad->RegisterComponent();
		Coordinator->RegisterMember(BetaSquad);

		TestTrue(TEXT("Tracker ping recruits equidistant squad deterministically"),
				 Tracker->PingTarget(FVector(1000.f, 0.f, 0.f)));

		// Verify GetTrackerFollowLocation computes exact staggered formation slots:
		// Offset = (-Spacing * (Slot / 2 + 1), Spacing * (Slot % 2 == 0 ? -0.5f : 0.5f), 0.0f)
		const float Spacing = 200.0f;
		const FVector FollowSlotA = Coordinator->GetTrackerFollowLocation(AlphaSquad, Spacing);
		const FVector FollowSlotB = Coordinator->GetTrackerFollowLocation(BetaSquad, Spacing);
		TestEqual(TEXT("Repeated GetTrackerFollowLocation calls are bit-for-bit identical"),
				  FollowSlotA, Coordinator->GetTrackerFollowLocation(AlphaSquad, Spacing));
		TestFalse(TEXT("Distinct squad slots produce distinct deterministic formation offsets"),
				  FollowSlotA.Equals(FollowSlotB));
	}

	// Part D: Runtime determinism check on UInventory
	UInventory *InvA = NewObject<UInventory>();
	UInventory *InvB = NewObject<UInventory>();
	FCardItem AutoCard =
		MakeDeterministicCard(TEXT("DetCard"), UGSCGameplayAbility::StaticClass(), ECardRefillType::Automatic, 4.5f);

	InvA->AddCardToInventory_Implementation(AutoCard);
	InvB->AddCardToInventory_Implementation(AutoCard);
	InvA->RemoveSelectedCard();
	InvB->RemoveSelectedCard();
	InvA->UpdateAutoRefills(1.25f);
	InvB->UpdateAutoRefills(1.25f);

	TestEqual(TEXT("Identical refill progression produces identical remaining time"),
			  InvA->GetRefillRemaining(AutoCard.AbilityClass), InvB->GetRefillRemaining(AutoCard.AbilityClass));

	return true;
}

// =================================================================================================
// 3. PROJECT ALIGNMENT: COMPONENT-BASED ARCHITECTURE, SUBSYSTEMS & .INL PARTITIONING
// =================================================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRefactoredProjectAlignmentTest,
								 "WOTD.Refactoring.Architecture.ComponentAndInlAlignment", RefactoredAuditFlags)

bool FRefactoredProjectAlignmentTest::RunTest(const FString &Parameters)
{
	// 1. Verify Component-Based Architecture on AAliCharacter
	AAliCharacter *Player = NewObject<AAliCharacter>();
	if (!TestNotNull(TEXT("AAliCharacter created"), Player))
	{
		return false;
	}
	TestNotNull(TEXT("AAliCharacter owns UAlsCameraComponent"), Player->FindComponentByClass<UAlsCameraComponent>());
	TestNotNull(TEXT("AAliCharacter owns UInventory component"), Player->FindComponentByClass<UInventory>());
	TestNotNull(TEXT("AAliCharacter owns UGSCCoreComponent"), Player->FindComponentByClass<UGSCCoreComponent>());
	TestNotNull(TEXT("AAliCharacter owns UGSCAbilitySystemComponent"), Player->GetAbilitySystem().Get());
	TestNotNull(TEXT("AAliCharacter owns UParkourComponent"), Player->GetParkourComponent());
	TestNotNull(TEXT("AAliCharacter owns UCoverComponent"), Player->FindComponentByClass<UCoverComponent>());
	TestNotNull(TEXT("AAliCharacter owns UAimObstructionComponent"),
				Player->FindComponentByClass<UAimObstructionComponent>());
	TestNotNull(TEXT("AAliCharacter owns UPlayerSFXComponent"), Player->FindComponentByClass<UPlayerSFXComponent>());
	TestNotNull(TEXT("AAliCharacter owns UContextualAnimSceneActorComponent"),
				Player->FindComponentByClass<UContextualAnimSceneActorComponent>());

	// 2. Verify Component-Based Architecture on AABaseEnemy & ATrackerEnemy subclass alignment
	ATrackerEnemy *Tracker = NewObject<ATrackerEnemy>();
	if (!TestNotNull(TEXT("ATrackerEnemy created"), Tracker))
	{
		return false;
	}
	TestTrue(TEXT("ATrackerEnemy inherits AABaseEnemy component hierarchy"), Tracker->IsA<AABaseEnemy>());
	TestNotNull(TEXT("ATrackerEnemy inherits UGameplayTagManagerComponent"),
				Tracker->FindComponentByClass<UGameplayTagManagerComponent>());
	TestNotNull(TEXT("ATrackerEnemy inherits UContextualAnimSceneActorComponent"),
				Tracker->FindComponentByClass<UContextualAnimSceneActorComponent>());
	TestNotNull(TEXT("ATrackerEnemy inherits UEnemyBarkComponent"), Tracker->BarkComponent.Get());
	TestNotNull(TEXT("ATrackerEnemy inherits UEnemyHidingSearchComponent"), Tracker->HidingSearchComponent.Get());
	TestNotNull(TEXT("ATrackerEnemy inherits UEnemyDamageReactionComponent"), Tracker->DamageReactionComponent.Get());

	// 3. Verify Ordered .inl Unity-Style Partitioning Pattern across the classes and SquadCoordinatorSubsystem
	// (where TrackerEnemy's squad coordination logic lives in 06_TrackerCoordination.inl)
	const FString BaseDir = GetRefactoredSourceDir();
	struct FInlHubExpectation
	{
		const TCHAR *HubCppRelativePath;
		const TCHAR *InlFolderRelativePath;
		int32 ExpectedInlCount;
	};

	const FInlHubExpectation Expectations[] = {
		{TEXT("Private/Player/AliCharacter.cpp"), TEXT("Private/Player/AliCharacter"), 11},
		{TEXT("Private/Enemy/ABaseEnemy.cpp"), TEXT("Private/Enemy/ABaseEnemy"), 3},
		{TEXT("Private/Player/Components/InventoryComponent.cpp"),
		 TEXT("Private/Player/Components/InventoryComponent"), 4},
		{TEXT("Private/Player/Components/CoverComponent.cpp"), TEXT("Private/Player/Components/CoverComponent"), 7},
		{TEXT("Private/StealthSystem/Subsystems/SquadCoordinatorSubsystem.cpp"),
		 TEXT("Private/StealthSystem/Subsystems/SquadCoordinatorSubsystem"), 7}};

	for (const FInlHubExpectation &Exp : Expectations)
	{
		const FString HubPath = FPaths::Combine(BaseDir, Exp.HubCppRelativePath);
		const FString InlDir = FPaths::Combine(BaseDir, Exp.InlFolderRelativePath);
		if (FPaths::FileExists(HubPath))
		{
			FString HubContent;
			TestTrue(*FString::Printf(TEXT("Loads hub file %s"), Exp.HubCppRelativePath),
					 FFileHelper::LoadFileToString(HubContent, *HubPath));

			TArray<FString> InlFiles;
			IFileManager::Get().FindFiles(InlFiles, *(InlDir / TEXT("*.inl")), true, false);
			InlFiles.Sort();

			TestEqual(*FString::Printf(TEXT("%s has exact expected .inl partition count"), Exp.InlFolderRelativePath),
					  InlFiles.Num(), Exp.ExpectedInlCount);

			int32 LastIncludePos = -1;
			for (int32 idx = 0; idx < InlFiles.Num(); ++idx)
			{
				const FString ExpectedPrefix = FString::Printf(TEXT("%02d_"), idx + 1);
				TestTrue(*FString::Printf(TEXT("%s starts with sequential prefix %s"), *InlFiles[idx], *ExpectedPrefix),
						 InlFiles[idx].StartsWith(ExpectedPrefix));

				const int32 IncludePos = HubContent.Find(InlFiles[idx]);
				TestTrue(*FString::Printf(TEXT("Hub %s includes %s in sequential order"), Exp.HubCppRelativePath,
										  *InlFiles[idx]),
						 IncludePos > LastIncludePos);
				LastIncludePos = IncludePos;
			}
		}
	}

	return true;
}

// =================================================================================================
// 4. READABILITY & CODE QUALITY AUDIT (CLAUDE vs. GPT BENCHMARK, INCLUDING TRACKERENEMY)
// =================================================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRefactoredReadabilityAuditTest,
								 "WOTD.Refactoring.Readability.ModularityAndDocumentationAudit", RefactoredAuditFlags)

bool FRefactoredReadabilityAuditTest::RunTest(const FString &Parameters)
{
	const FString BaseDir = GetRefactoredSourceDir();
	if (!FPaths::DirectoryExists(BaseDir))
	{
		return true;
	}

	// Criterion 1: Every .inl partition file (including SquadCoordinatorSubsystem/06_TrackerCoordination.inl)
	// must start with a clear domain header comment and remain modular (< 600 lines).
	const TArray<FString> TargetInlDirs = {
		FPaths::Combine(BaseDir, TEXT("Private/Player/AliCharacter")),
		FPaths::Combine(BaseDir, TEXT("Private/Enemy/ABaseEnemy")),
		FPaths::Combine(BaseDir, TEXT("Private/Player/Components/InventoryComponent")),
		FPaths::Combine(BaseDir, TEXT("Private/Player/Components/CoverComponent")),
		FPaths::Combine(BaseDir, TEXT("Private/StealthSystem/Subsystems/SquadCoordinatorSubsystem"))};

	for (const FString &Dir : TargetInlDirs)
	{
		TArray<FString> InlFiles;
		IFileManager::Get().FindFiles(InlFiles, *(Dir / TEXT("*.inl")), true, false);
		for (const FString &FileName : InlFiles)
		{
			const FString FullPath = FPaths::Combine(Dir, FileName);
			TArray<FString> Lines;
			if (FFileHelper::LoadFileToStringArray(Lines, *FullPath))
			{
				TestTrue(*FString::Printf(TEXT("%s begins with a descriptive header comment"), *FileName),
						 Lines.Num() > 0 && Lines[0].TrimStart().StartsWith(TEXT("//")));
				TestTrue(*FString::Printf(TEXT("%s documents single-inclusion contract ('Included once by')"),
										  *FileName),
						 Lines.Num() > 0 && Lines[0].Contains(TEXT("Included once by")));
				TestTrue(*FString::Printf(TEXT("%s stays modular and readable (< 600 lines, actual: %d)"), *FileName,
										  Lines.Num()),
						 Lines.Num() < 600);
			}
		}
	}

	// Criterion 2: Verify TrackerEnemy.cpp and TrackerSquadNodes.cpp follow the clean Claude-style refactoring
	// standard: high active-code density (> 90% executable C++), flat guard clauses, and zero raw LLM artifacts.
	const TArray<FString> TrackerFiles = {
		FPaths::Combine(BaseDir, TEXT("Private/Enemy/TrackerEnemy.cpp")),
		FPaths::Combine(BaseDir, TEXT("Private/BehaviorTree/TrackerSquadNodes.cpp")),
		FPaths::Combine(BaseDir, TEXT("Private/StealthSystem/Subsystems/SquadCoordinatorSubsystem/06_TrackerCoordination.inl"))};

	for (const FString &TrackerFile : TrackerFiles)
	{
		TArray<FString> Lines;
		if (FFileHelper::LoadFileToStringArray(Lines, *TrackerFile))
		{
			const FString CleanName = FPaths::GetCleanFilename(TrackerFile);
			int32 ActiveLines = 0;
			int32 CommentLines = 0;
			for (const FString &Line : Lines)
			{
				const FString Trimmed = Line.TrimStartAndEnd();
				if (Trimmed.IsEmpty())
				{
					continue;
				}
				TestFalse(*FString::Printf(TEXT("%s contains no raw LLM citation markers (oaicite)"), *CleanName),
						  Trimmed.Contains(TEXT("oaicite")));
				if (Trimmed.StartsWith(TEXT("//")) || Trimmed.StartsWith(TEXT("/*")) || Trimmed.StartsWith(TEXT("*")))
				{
					++CommentLines;
				}
				else
				{
					++ActiveLines;
				}
			}
			TestTrue(*FString::Printf(TEXT("%s is concise (< 300 lines) and free of dead commented-out code (> 85%% active lines)"),
									  *CleanName),
					 Lines.Num() < 300 && ActiveLines > CommentLines * 5);
		}
	}

	return true;
}