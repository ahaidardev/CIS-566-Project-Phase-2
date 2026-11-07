// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/FInventoryItem.h"
#include "Data/FCardItem.h"
// #include "Interfaces/BPI_AbilityCard.h"
#include "Player/Interfaces/BPI_AbilityCard.h"
#include "Abilities/GSCGameplayAbility.h"
#include "InputCoreTypes.h"
#include "InventoryComponent.generated.h"
class UCard;
class UCardWheel;
class UCardInventoryPanel;

USTRUCT()
struct FPendingCardRefill
{
	GENERATED_BODY()
	UPROPERTY()
	FCardItem Card;
	UPROPERTY()
	int32 MissingCharges = 0;
	UPROPERTY()
	float RemainingTime = 0;
};

// dispatcher declarations
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardAddedSignature, FCardItem, Card);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardRemovedSignature, FCardItem, Card);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardSelectedSignature, int32, CurrentSlotIndex);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class WOTDREFACTORED_API UInventory : public UActorComponent, public IBPI_AbilityCard
{
	GENERATED_BODY()

  public:
	UFUNCTION(BlueprintCallable, Category = "Cards")
	void RefreshCardOverlay();
	// Sets default values for this component's properties
	UInventory();

	// UPROPERTY(BlueprintReadOnly)
	// FCardItem SelectedCard; // Dont use this cause its not changing.

	// dispatcher definitions
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCardAddedSignature OnCardAdded;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCardSelectedSignature OnCardSelected;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCardRemovedSignature OnCardRemoved;

  protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,

							   FActorComponentTickFunction* ThisTickFunction) override;

	// Getters
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetNextCard();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetPreviousCard();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<FCardItem> GetUniqueCards();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TMap<TSubclassOf<UGSCGameplayAbility>, int32> GetCardCopyCounts();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FCardItem GetCardData(TSubclassOf<UGSCGameplayAbility> AbilityClass);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<TSubclassOf<UGSCGameplayAbility>> GetSlotOrder();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetCurrentSlotIndex();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool CanAddCard(const FCardItem& Card);

	// action functions
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SelectCard(int32 CardIndex);

	virtual FCardItem AddCardToInventory_Implementation(const FCardItem& Card);
	// FCardItem AddCardToInventory_Implementation(const FCardItem& Card);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FCardItem RemoveSelectedCard();

	void UpdateAutoRefills(float DeltaTime);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	float GetRefillRemaining(TSubclassOf<UGSCGameplayAbility> AbilityClass) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<FCardItem> GetWheelCards() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<FCardItem> GetReserveCards() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SwapSelectedCardWithReserve(TSubclassOf<UGSCGameplayAbility> ReserveClass);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetReserveCardCount(TSubclassOf<UGSCGameplayAbility> AbilityClass) const;

	// variables
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TMap<TSubclassOf<UGSCGameplayAbility>, int32> CardCopyCounts;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TMap<TSubclassOf<UGSCGameplayAbility>, FCardItem> CardData;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TArray<TSubclassOf<UGSCGameplayAbility>> SlotOrder;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FInventoryItem> MyItems;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 CurrentSlotIndex;

	UPROPERTY(BlueprintReadWrite)
	bool bIsRecycling = false;

	/** Toggle the equipped wheel into a description hand without changing inventory. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cards|Input")
	FKey CardHandKeyboardKey = EKeys::H;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cards|Input")
	FKey CardHandControllerKey = EKeys::Gamepad_DPad_Up;

  private:
	UPROPERTY(Transient)
	TMap<TSubclassOf<UGSCGameplayAbility>, FPendingCardRefill> PendingRefills;
	UPROPERTY(Transient)
	TMap<TSubclassOf<UGSCGameplayAbility>, int32> ReserveCardCounts;
	UPROPERTY(Transient)
	TMap<TSubclassOf<UGSCGameplayAbility>, FCardItem> ReserveCardData;

	UPROPERTY(Transient)
	TObjectPtr<UCardWheel> CardWheel;

	UPROPERTY(Transient)
	TObjectPtr<UCardInventoryPanel> CardInventoryPanel;

	bool bWasWheelHeld = false;
	bool bWasReserveWheelHeld = false;
	bool bWasInventoryKeyDown = false;
	bool bWasHandToggleKeyDown = false;
	void UpdateCardWheel();

	class AAliCharacter* OwnerChar;
	class UGSCAbilitySystemComponent* ASC;

	void ConsumePendingRefillCharge(const FCardItem& Card);
	void PlaceCardInWheelOrReserve(const FCardItem& Card);
	void ScheduleAutoRefill(const TSubclassOf<UGSCGameplayAbility>& SelectedClass, const FCardItem& RemovedCard);
	void RemoveSlotAndClampSelection(const TSubclassOf<UGSCGameplayAbility>& SelectedClass);
	FPendingCardRefill* GrantDueRefillCharges(FPendingCardRefill* Pending, const TSubclassOf<UGSCGameplayAbility>& Key);
	void MoveEquippedStackToReserve(const TSubclassOf<UGSCGameplayAbility>& EquippedClass);
	void EquipReserveStack(const TSubclassOf<UGSCGameplayAbility>& ReserveClass, int32 ReserveCount,
						   const FCardItem& ReserveData);
	void TearDownCardWidgets();
	void AcquireCardWheel(class APlayerController* Controller);
	void UpdateInventoryPanel(class APlayerController* Controller, bool bInventoryKeyDown);
	void UpdateHandLayoutToggle(class APlayerController* Controller);
	void UpdateWheelInputAndAnimation(class APlayerController* Controller, bool bWheelHeld);
};
