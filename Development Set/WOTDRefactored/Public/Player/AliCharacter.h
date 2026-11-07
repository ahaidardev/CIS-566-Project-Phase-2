// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AlsCharacter.h"

#include "Engine/LocalPlayer.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerController.h"
#include "AlsAnimationInstance.h"
#include "AlsCharacterExample.h"
#include "AlsCharacterMovementComponent.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/GameNetworkManager.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
// #include "Components/AlsCharacterSettings.h"
#include "Utility/AlsConstants.h"
#include "Utility/AlsMacros.h"
#include "Utility/AlsUtility.h"
#include "Components/GSCCoreComponent.h"
#include "Perception/AISightTargetInterface.h"
// #include "Components/GSCAbilitySystemComponent.h"
#include "AliCharacter.generated.h"

namespace EDrawDebugTrace
{
enum Type : int;
}
class UParkourComponent;
class UGSCCoreComponent;
class UGSCAbilitySystemComponent;
class UAlsCameraComponent;
struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
class UAlsMovementSettings;
class UInventory;
class UContextualAnimSceneActorComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class UMeshComponent;
class UGroomComponent;
class UAimObstructionComponent;
class UPlayerSFXComponent;

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityEnded, TSubclassOf<UGameplayAbility>, AbilityClass);
// UPROPERTY(BlueprintAssignable)
// FOnAbilityEnded OnAbilityEnded;

UCLASS()
class WOTDREFACTORED_API AAliCharacter : public AAlsCharacter, public IAISightTargetInterface
{
	GENERATED_BODY()

  public:
	bool IsManualAimRequested() const
	{
		return bManualAimRequested;
	}

  private:
	bool bManualAimRequested = false;

  public:
	virtual UAISense_Sight::EVisibilityResult CanBeSeenFrom(
		const FCanBeSeenFromContext& Context, FVector& OutSeenLocation, int32& OutNumberOfLoSChecksPerformed,
		int32& OutNumberOfAsyncLosCheckRequested, float& OutSightStrength, int32* UserData = nullptr,
		const FOnPendingVisibilityQueryProcessedDelegate* Delegate = nullptr) override;

  protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ali Character")
	bool bIsAiming;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ali Character")
	TObjectPtr<UAlsCameraComponent> Camera;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UInventory> InventoryComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UContextualAnimSceneActorComponent> CASComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover", meta = (AllowPrivateAccecss = "true"))
	class UCoverComponent* CoverComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Parkour", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UParkourComponent> ParkourComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aiming")
	TObjectPtr<UAimObstructionComponent> AimObstructionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Voice")
	TObjectPtr<UPlayerSFXComponent> PlayerSFXComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TObjectPtr<UGSCCoreComponent> GSCCoreComp;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGSCAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ali Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ali Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> LookMouseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ali Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ali Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ali Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ali Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> WalkAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ali Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ali Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ali Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ali Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> RagdollAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ali Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> RollAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ali Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> RotationModeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ali Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> ViewModeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ali Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> SwitchShoulderAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ali Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> ShootAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ali Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> CardSelectLeft;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ali Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> CardSelectRight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ali Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> ParkourAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Ali Character",
			  Meta = (ClampMin = 0, ForceUnits = "x"))
	float LookUpMouseSensitivity{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Ali Character",
			  Meta = (ClampMin = 0, ForceUnits = "x"))
	float LookRightMouseSensitivity{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Ali Character",
			  Meta = (ClampMin = 0, ForceUnits = "deg/s"))
	float LookUpRate{90.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Ali Character",
			  Meta = (ClampMin = 0, ForceUnits = "deg/s"))
	float LookRightRate{240.0f};

	//	UFUNCTION(BlueprintCallable, Category = "ONO_Settings", Meta = (DisplayThumbnail = false))
	//		void Input_OnAssassination();

  protected:
	// UFUNCTION(BlueprintNativeEvent, Category = "Als Character")
	//UAlsMantlingSettings* SelectMantlingSettings(EAlsMantlingType MantlingType);

	// UFunction(BlueprintCallable, Catergory="ONO_Settings", Meta = (DisplayThumbnail = false) )

	// UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ali Character")
	// TObjectPtr<UONO_BaseAbilityChip>

	// ========== GRID ==========

	// ========== THROWABLE/PROJECTILE SYSTEM ==========

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	TObjectPtr<AActor> NoiseMaker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	int32 TeleportChipNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	int32 SmokeBombNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	TObjectPtr<UAnimMontage> Montage_Throw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	TSoftObjectPtr<UActorComponent> TeleportChipRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	FVector ChipStartVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	TSoftObjectPtr<AActor> ThrowableRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	double PowerShotIncrementTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	double PowerShotTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	FTimerHandle PowerShotTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	double ElapsedSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	double PlayerThrowStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	double MaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	double MinSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	double MinGravity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	double MaxGravity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	bool bIsFiringCard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	double PowerShotTimerValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	FVector ProjectileVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	FVector FastProjectileVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	FTransform ProjectileSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	FVector2D CenterOfScreen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	TSubclassOf<AActor> HitGrid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	FVector HitLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwables")
	FVector CenterScreenPosition;

	// ========== OVERLAY/EQUIPMENT SYSTEM ==========
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlay")
	// TObjectPtr<UStaticMeshComponent> OverlayStaticMesh;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlay")
	// TObjectPtr<USkeletalMeshComponent> OverlaySkeletalMesh;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlay")
	// FName ResultSocketName;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlay")
	// bool bUseGunBoneForOverlayObjects;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlay")
	// TMap<FGameplayTag, TSubclassOf<UAnimInstance>> OverlayAnimationInstanceClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlay")
	TSubclassOf<UAnimInstance> OverlayAnimationInstanceClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlay")
	TObjectPtr<USkeletalMesh> NewSkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlay")
	TSubclassOf<UAnimInstance> NewAnimationClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlay")
	TObjectPtr<UStaticMesh> NewStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlay")
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlay")
	bool bUseLeftGunBone;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlay")
	// double PlayRate;
	// ========== HEALTH SYSTEM ==========
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	double Health;
	// ========== ABILITY/STEALTH SYSTEM ==========
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	double InvisibilityTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	bool bIsGrappling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TArray<TObjectPtr<UMaterialInstance>> AliMatRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TArray<TObjectPtr<UMaterialInstanceDynamic>> AliDynMatRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TObjectPtr<UMaterialInstanceDynamic> AliDynMask;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TObjectPtr<UMaterialInstanceDynamic> AliDynHair;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TSoftObjectPtr<AActor> NewVar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TObjectPtr<USkinnedMeshComponent> NewLeaderBoneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TArray<TEnumAsByte<EObjectTypeQuery>> InteractableObjectTypes;
	int CASComp;

	// ========== BLUEPRINT IMPLEMENTABLE EVENTS ==========

	//Implement the following Classes
	/*
	UFUNCTION(BlueprintImplementableEvent, Category = "Character")
	void OnFellOutOfWorld();

	UFUNCTION(BlueprintImplementableEvent, Category = "Montage")
	void PlayMontage();

	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
	void Reveal();

	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
	void Conceal();

	UFUNCTION(BlueprintImplementableEvent, Category = "Takedown")
	void TakedownTrace();

	UFUNCTION(BlueprintImplementableEvent, Category = "Aiming")
	void StartAim();

	UFUNCTION(BlueprintImplementableEvent, Category = "Cover")
	void ResetCover();

	UFUNCTION(BlueprintImplementableEvent, Category = "Cover")
	void ResetCoverVault();

	UFUNCTION(BlueprintImplementableEvent, Category = "Cover")
	void InitCover();

	UFUNCTION(BlueprintImplementableEvent, Category = "Cover")
	void DetachFromCover();

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void AddCard(const FString& CardName);

	UFUNCTION(BlueprintImplementableEvent, Category = "Threat")
	void CreateThreat();

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void PickUpRock();

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void PickUpSmokeBomb();

	UFUNCTION(BlueprintImplementableEvent, Category = "Stealth")
	void StealthInterfaceStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Stealth")
	void StealthInterfaceEnd();

	UFUNCTION(BlueprintImplementableEvent, Category = "Shooting")
	void PowerShot();

	UFUNCTION(BlueprintImplementableEvent, Category = "Stealth")
	void StealthInterfaceSpotted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Stealth")
	void StealthInterfaceLost();

	UFUNCTION(BlueprintImplementableEvent, Category = "Stealth")
	void StealthInterfaceCanAssassinate(bool bBehind);

	UFUNCTION(BlueprintImplementableEvent, Category = "Stealth")
	void StealthInterfaceStatus(int32 Status, AActor* Actor);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void StealthInterfaceDeleteSplines(const TArray<USplineMeshComponent*>& SplineArray);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void StealthInterfaceDisplayHelpText(bool bOffOn, const FText& Text, bool bIsAssassination, int32 Type);

	// UFUNCTION(BlueprintCallable, Category = "Aiming")
	// void StopAiming();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ClearSplines(const TArray<USplineMeshComponent*>& SplinesToClear);

	UFUNCTION(BlueprintImplementableEvent, Category = "Ragdoll")
	void OnRagdollingStartedBP();

	UFUNCTION(BlueprintImplementableEvent, Category = "Damage")
	void PointDamage(float Damage, const UDamageType* DamageType, FVector InHitLocation, FVector InHitNormal, UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, AController* InstigatedBy, AActor* DamageCauser, const FHitResult& HitInfo);

	UFUNCTION(BlueprintImplementableEvent, Category = "Vault")
	void CheckGapSizeEvent();

	UFUNCTION(BlueprintImplementableEvent, Category = "Damage")
	void AnyDamage(float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void HealthCheck();

	UFUNCTION(BlueprintImplementableEvent, Category = "Rotation")
	void SetTargetRotation();

	UFUNCTION(BlueprintImplementableEvent, Category = "Rotation")
	void RotateCharacter();

	UFUNCTION(BlueprintImplementableEvent, Category = "Overlay")
	void OnOverlayModeChangedBP(const FGameplayTag& PreviousOverlayMode);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cards")
	void InitRegularCardThrow();

	// ========== C++ CALLABLE FUNCTIONS ==========
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddToInventory(uint8 Type);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RemoveFromInventory();

	UFUNCTION(BlueprintCallable, Category = "Throwables")
	void SpawnProjectile(FVector Velocity, double Gravity);

	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void SetAimingVariables(double Gravity, FVector Velocity);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateInventoryUI();

	UFUNCTION(BlueprintCallable, Category = "Shooting")
	void SetShotPower(double Strength, bool NewParam);

	UFUNCTION(BlueprintCallable, Category = "Shooting")
	void FirePowerShotEnd();

	UFUNCTION(BlueprintCallable, Category = "Throwables")
	void ThrowTeleportChip();

	UFUNCTION(BlueprintCallable, Category = "Shooting")
	void NormalWeaponShot();

	UFUNCTION(BlueprintCallable, Category = "Shooting")
	void PulseShotFunction(bool NewParam);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void WeaponAimTrace(FVector& TraceStart, FVector& InTraceEnd, FHitResult& OutHit);

	UFUNCTION(BlueprintCallable, Category = "Throwables")
	void ThrowFireChip();

	UFUNCTION(BlueprintCallable, Category = "Throwables")
	void ThrowTranq(FVector Velocity);

	UFUNCTION(BlueprintCallable, Category = "Throwables")
	void ThrowCloneChip();

	UFUNCTION(BlueprintCallable, Category = "Throwables")
	void CalculateProjectileSpawn(FVector InTraceEnd);

	UFUNCTION(BlueprintCallable, Category = "Character")
	void KO();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ItemTrace(UActorComponent*& StealthComponent, AActor*& OutHitActor);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool EnemyTrace(FHitResult& OutHit);

	UFUNCTION(BlueprintCallable, Category = "Throwables")
	void ThrowRock();

	UFUNCTION(BlueprintCallable, Category = "Cards")
	void UseSelectedCard();

	UFUNCTION(BlueprintCallable, Category = "Cards")
	FString GetCardName(UUserWidget* self2);

	UFUNCTION(BlueprintCallable, Category = "Vault")
	void VaultMotionWarp();

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void CameraChanges();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void RefreshOverlayLinkedAnimationLayer();

	UFUNCTION(BlueprintCallable, Category = "Vault")
	void CheckIfObjectExists(bool& bOutObstacleExists, FRotator& OutTargetRotation);

	UFUNCTION(BlueprintCallable, Category = "Vault")
	void CheckObsThicknessEvent();

	UFUNCTION(BlueprintCallable, Category = "Vault")
	void CheckObsThickness(TEnumAsByte<EDrawDebugTrace::Type> InDrawDebugType, double InForwardDistance, float Radius, FLinearColor InTraceColor, FLinearColor TraceHitColor, float InDrawTime);

	UFUNCTION(BlueprintCallable, Category = "Vault")
	void CheckObsHeightEvent();

	UFUNCTION(BlueprintCallable, Category = "Vault")
	void CheckObsHeight(double TraceHeight, float TraceForward, FLinearColor InTraceColor, FLinearColor TraceHitColor, TEnumAsByte<EDrawDebugTrace::Type> InDrawDebugType);

	UFUNCTION(BlueprintCallable, Category = "Vault")
	void CheckObsInfo(FVaultMantleInfo& VaultMantleStructParam);

	UFUNCTION(BlueprintCallable, Category = "Vault")
	void CheckObsGap(TEnumAsByte<EDrawDebugTrace::Type> InDrawDebugType);

	UFUNCTION(BlueprintCallable, Category = "Vault")
	void CheckObsGapPure(UCapsuleComponent* Capsule, double RadiusOffset, TEnumAsByte<EDrawDebugTrace::Type> InDrawDebugType, FLinearColor InTraceColor, FLinearColor TraceHitColor, FVector Start, FVector End, bool& bHasRoom, FVector& CheckGapTraceEndOut);

	UFUNCTION(BlueprintCallable, Category = "Vault")
	void CheckThickness();

	UFUNCTION(BlueprintCallable, Category = "Save")
	Fs_PlayerSave GetPlayerSave();

	UFUNCTION(BlueprintCallable, Category = "Save")
	void UsePlayerSave(Fs_PlayerSave PlayerSave);

	UFUNCTION(BlueprintCallable, Category = "Mantle")
	bool IsMantlingAllowedToStartBP();

	UFUNCTION(BlueprintCallable, Category = "Throwables")
	void ThrowShockChip(bool bPowerShot, FVector Velocity);

	UFUNCTION(BlueprintCallable, Category = "Cards")
	void AutoSelect();

	UFUNCTION(BlueprintCallable, Category = "Throwables")
	void ThrowGrenade();

	UFUNCTION(BlueprintCallable, Category = "Character")
	void HairLODSetup();

	UFUNCTION(BlueprintCallable, Category = "Cards")
	void DiscardSelectedCard();

	UFUNCTION(BlueprintCallable, Category = "Shooting")
	void PowerWeaponShot();

	UFUNCTION(BlueprintCallable, Category = "Throwables")
	void ThrowSmoke();

	UFUNCTION(BlueprintCallable, Category = "Trace")
	void TraceReq();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void EnableElectroShield();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void DisableElectricShield();

	UFUNCTION(BlueprintCallable, Category = "Cards")
	void EquipCard();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void AbsorbShard();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void EnableInvisibility();

	UFUNCTION(BlueprintCallable, Category = "Grapple")
	bool GrappleGetTargetInfo();

	UFUNCTION(BlueprintCallable, Category = "Grapple")
	bool GrappleIdentifyTarget();

	UFUNCTION(BlueprintCallable, Category = "Grapple")
	void GrappleCalculatePlayerGrappleTargetLocation();

	UFUNCTION(BlueprintCallable, Category = "Grapple")
	void GrappleSetActorLocation(float Alpha);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void HideMeshFx(float Appearance);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void DisableInvisibility();

	*/

  public:
	AAliCharacter();

	UFUNCTION(BlueprintCallable, Category = "Abilities|Invisibility")
	void InitializeDynamicMaterials();

	UFUNCTION(BlueprintCallable, Category = "Abilities|Invisibility")
	void BecomeInvisible();

	UFUNCTION(BlueprintCallable, Category = "Abilities|Invisibility")
	void BecomeVisible();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities|Invisibility", meta = (ClampMin = "0.0"))
	float InvisibilityTransitionDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities|Invisibility")
	float InvisibleAppearance = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Invisibility")
	TMap<TObjectPtr<UMaterialInterface>, TObjectPtr<UMaterialInterface>> InvisibilityMaterialVariants;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Abilities|Invisibility")
	float InvisibilityAlpha = 0.0f;

	virtual void NotifyControllerChanged() override;

	void SetCameraRightShoulder(bool bRightShoulder);

	bool GetCameraRightShoulder();

	TObjectPtr<UGSCAbilitySystemComponent> GetAbilitySystem()
	{
		return AbilitySystemComponent;
	}

	UFUNCTION()
	UParkourComponent* GetParkourComponent()
	{
		return ParkourComp;
	}

  private:
	void StartInvisibilityTransition(float TargetAlpha);
	void UpdateInvisibilityTransition();
	void ApplyInvisibilityAppearance();
	FTimerHandle InvisibilityTransitionTimer;
	float InvisibilityTargetAlpha = 0.0f;
	float InvisibilityStartAlpha = 0.0f;
	double InvisibilityTransitionStartTime = 0.0;
	TMap<TWeakObjectPtr<UMaterialInstanceDynamic>, float> VisibleMaterialAppearance;
	// Retained for Live Coding layout compatibility; invisibility never changes visibility.
	TMap<TWeakObjectPtr<UMeshComponent>, bool> PreInvisibilityMeshVisibility;
	struct FInvisibilityHairWidth
	{
		float OriginalWidth;
		float EffectiveWidth;
		bool bOriginalOverride;
	};
	TMap<TWeakObjectPtr<UGroomComponent>, TArray<FInvisibilityHairWidth>> VisibleGroomWidths;

	virtual bool IsMantlingAllowedToStart_Implementation() const override;

	// Camera

  protected:
	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& ViewInfo) override;

	// Input

  protected:
	virtual void SetupPlayerInputComponent(UInputComponent* Input) override;

  private:
	void Input_OnSprintEnd(const FInputActionValue& InputActionValue);
	void Input_OnCardSelectLeft(const FInputActionValue& ActionValue);

	void Input_OnCardSelectRight(const FInputActionValue& ActionValue);

	void Input_OnLookMouse(const FInputActionValue& ActionValue);

	void Input_OnLook(const FInputActionValue& ActionValue);

	void Input_OnMove(const FInputActionValue& ActionValue);

	void Input_OnSprint(const FInputActionValue& ActionValue);

	void Input_OnWalk();

	void Input_OnCrouch();

	void Input_OnJump(const FInputActionValue& ActionValue);

	void Input_OnAim(const FInputActionValue& ActionValue);

	void Input_OnRagdoll();

	void Input_OnRoll();

	void Input_OnRotationMode();
	UAlsMantlingSettings* SelectMantlingSettings(EAlsMantlingType MantlingType);

	// void Input_OnViewMode();

	void Input_OnSwitchShoulder();

	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Input_OnCoverMove(const FInputActionValue& ActionValue);
	void Input_OnCoverMoveEnd();
	// void Input_OnShoot();
	// void SetOverlayMode(const FGameplayTag& NewOverlayMode, bool bSendRpc);
	// void OnReplicated_OverlayMode(const FGameplayTag& PreviousOverlayMode);
	void Input_OnParkour();

	// // Debug
  protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void NotifyLocomotionActionChanged(const FGameplayTag& PreviousLocomotionAction) override;

	bool bRollingStartedCrouched = false;
	bool bSprintInputActive = false;
	FGameplayTag StanceBeforeSprint;
	float RootMotionScaleBeforeRoll = 1.0f;
	void ThrowTeleportChip();
	void NormalWeaponShot();
	void PulseShotFunction(bool NewParam);
	void WeaponAimTrace(FVector& TraceStart, FVector& InTraceEnd, FHitResult& OutHit);
	void ThrowFireChip();
	void ThrowTranq(FVector Velocity);
	void ThrowCloneChip();
	void CalculateProjectileSpawn(FVector InTraceEnd);
	void KO();
	void ItemTrace(UActorComponent*& StealthComponent, AActor*& OutHitActor);
	bool EnemyTrace(FHitResult& OutHit);

	void PowerWeaponShot();
	void Input_OnViewMode();

	//bool OnMantlingStarted_Implementation();

  public:
	void Input_OnShoot();
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& Unused,
							  float& VerticalLocation) override;
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	void CheckIfObjectExists(bool& bOutObstacleExists, FRotator& OutTargetRotation);
};
