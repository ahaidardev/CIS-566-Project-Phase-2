// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AlsCharacter.h"
#include "ABaseEnemy.generated.h"

class UGameplayTagManagerComponent;
class UContextualAnimSceneActorComponent;
class UEnemyBarkComponent;
class UEnemyHidingSearchComponent;
class UEnemyDamageReactionComponent;

UCLASS()
class WOTDREFACTORED_API AABaseEnemy : public AAlsCharacter
{
	GENERATED_BODY()

  public:
	// Sets default values for this character's properties
	AABaseEnemy();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio|Barks")
	TObjectPtr<UEnemyBarkComponent> BarkComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Search")
	TObjectPtr<UEnemyHidingSearchComponent> HidingSearchComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage|Reaction")
	TObjectPtr<UEnemyDamageReactionComponent> DamageReactionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Stealth", meta = (ClampMin = "1"))
	float PatrolWalkSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Stealth", meta = (ClampMin = "1"))
	float InvestigationJogSpeed = 100.0f;

	// Fallback for enemies without a Combat Role component; that component owns its combat speeds.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Combat", meta = (ClampMin = "1"))
	float AttackWalkForwardSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Combat", meta = (ClampMin = "1"))
	float AttackJogSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Stealth", meta = (ClampMin = "0.01"))
	float TurnInterpolationHalfLife = 0.12f;

  protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual bool RefreshCustomGroundedMovingRotation(float DeltaTime) override;
	virtual bool RefreshCustomGroundedNotMovingRotation(float DeltaTime) override;

  public:
	// Called every frame
	/** Intentional blast velocity bypasses ALS's brief accidental-ragdoll-speed clamp. */
	UFUNCTION(BlueprintCallable, Category = "Damage|Grenade")
	void ApplyGrenadeBlast(FVector Origin, float Radius = 1200.0f, float Strength = 1400.0f);

	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
							 AActor* DamageCauser) override;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	bool TryInterceptIncomingDamage(float Damage, AActor* Source, const FHitResult& Hit, bool bHasHit);
	virtual bool TryInterceptIncomingDamage_Implementation(float Damage, AActor* Source, const FHitResult& Hit,
														   bool bHasHit);

  protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Suspicion")
	TObjectPtr<UGameplayTagManagerComponent> GameplayTagManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ContextualAnimation")
	TObjectPtr<UContextualAnimSceneActorComponent> CASComponent;

  private:
	void InitializeMovementSettings();
	void ConfigurePatrolMovementSpeeds();
	void ConfigureInvestigationMovementSpeeds();
	void ConfigureCombatMovementSpeeds();
	void ConfigureCombatPositioningMovementSpeeds();
	void RefreshStealthLocomotion();
	UAlsMovementSettings* SelectStealthMovementSettings(const bool bCombat, const bool bInvestigating);
	void ApplyCombatRoleSpeedsAndInvalidateMovementCache(const float Speed, const FGameplayTag RoleGait);
	bool SmoothEnemyRotation(float DeltaTime, bool bMoving);
	UPROPERTY(Transient)
	TObjectPtr<UAlsMovementSettings> PatrolMovementSettings;
	UPROPERTY(Transient)
	TObjectPtr<UAlsMovementSettings> InvestigationMovementSettings;
	UPROPERTY(Transient)
	TObjectPtr<UAlsMovementSettings> CombatPositioningMovementSettings;
	UPROPERTY(Transient)
	TObjectPtr<UAlsMovementSettings> CombatRoleMovementSettings;
	float AppliedCombatRoleSpeed = -1.0f;
	FGameplayTag AppliedCombatRoleGait;
	UPROPERTY(Transient)
	TObjectPtr<UAlsMovementSettings> AppliedMovementSettings;

	// Called to bind functionality to input
  public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
