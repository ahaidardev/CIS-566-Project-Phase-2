// InventoryComponent: LifecycleAndSelection. Included once by InventoryComponent.cpp; retains original definition order.

//#include "GameplayAbility.h"

// Sets default values for this component's properties
UInventory::UInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UInventory::BeginPlay()
{
	Super::BeginPlay();

	OwnerChar = Cast<AAliCharacter>(GetOwner());
	if (OwnerChar)
	{
		ASC = OwnerChar->GetAbilitySystem();

		if (ASC)
		{
			// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("ASC is seen by Inventory!"));
		}
	}
	// if (OwnerChar = Cast<AAliCharacter>(GetOwner()))
	// {
	// 	ASC = OwnerChar->GetAbilitySystem();
	// }
}

// Called every frame
void UInventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateAutoRefills(DeltaTime);
	UpdateCardWheel();
}

// Getters
TArray<FCardItem> UInventory::GetUniqueCards()
{
	TArray<FCardItem> UniqueCards;
	for (const TSubclassOf<UGSCGameplayAbility>& AbilityClass : SlotOrder)
	{
		if (CardData.Contains(AbilityClass))
		{
			UniqueCards.Add(CardData[AbilityClass]);
		}
	}
	return UniqueCards;
}

int32 UInventory::GetNextCard()
{
	int32 NextIndex = SlotOrder.IsEmpty() ? 0 : (CurrentSlotIndex + 1) % SlotOrder.Num();
	if (SlotOrder.IsValidIndex(NextIndex))
	{
		SelectCard(NextIndex);
		return CurrentSlotIndex;
	}
	return CurrentSlotIndex;
}

int32 UInventory::GetPreviousCard()
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
	//        FString::Printf(TEXT("GetPreviousCard CALLED - CurrentSlotIndex = %d"), CurrentSlotIndex));

	int32 PrevIndex = SlotOrder.IsEmpty() ? 0 : (CurrentSlotIndex + SlotOrder.Num() - 1) % SlotOrder.Num();
	if (SlotOrder.IsValidIndex(PrevIndex))
	{
		SelectCard(PrevIndex);
		return CurrentSlotIndex;
	}
	return CurrentSlotIndex;
}

bool UInventory::CanAddCard(const FCardItem& Card)
{
	if (!Card.AbilityClass)
		return false;
	// Count total cards across all slots
	int32 TotalCards = 0;
	for (const auto& Pair : CardCopyCounts)
	{
		TotalCards += Pair.Value;
	}
	TSet<TSubclassOf<UGSCGameplayAbility>> ReservedClasses;
	for (const auto& Pair : CardCopyCounts)
		ReservedClasses.Add(Pair.Key);
	for (const auto& Pair : PendingRefills)
	{
		TotalCards += Pair.Value.MissingCharges;
		ReservedClasses.Add(Pair.Key);
	}
	// A pickup can satisfy a missing charge even when all capacity is reserved.
	if (PendingRefills.Contains(Card.AbilityClass))
		return true;
	for (const auto& Pair : ReserveCardCounts)
	{
		TotalCards += Pair.Value;
		ReservedClasses.Add(Pair.Key);
	}

	// Also check unique slot limit is still 5
	if (!CardCopyCounts.Contains(Card.AbilityClass))
	{
		return TotalCards < 15;
	}

	return TotalCards < 15;
}

TMap<TSubclassOf<UGSCGameplayAbility>, int32> UInventory::GetCardCopyCounts()
{
	return CardCopyCounts;
}

FCardItem UInventory::GetCardData(TSubclassOf<UGSCGameplayAbility> AbilityClass)
{
	if (CardData.Contains(AbilityClass))
	{
		return CardData[AbilityClass];
	}
	return {};
}

TArray<TSubclassOf<UGSCGameplayAbility>> UInventory::GetSlotOrder()
{
	return SlotOrder;
}

int32 UInventory::GetCurrentSlotIndex()
{
	return CurrentSlotIndex;
}

// action functions
void UInventory::SelectCard(int32 SlotIndex)
{
	if (SlotOrder.IsEmpty())
		return;
	if (!SlotOrder.IsValidIndex(SlotIndex))
		return;

	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
	//        FString::Printf(TEXT("CurrentSlotIndex was %d"),
	//        CurrentSlotIndex));

	CurrentSlotIndex = SlotIndex;
	OnCardSelected.Broadcast(CurrentSlotIndex);
}

void UInventory::RefreshCardOverlay()
{
	AAliCharacter* Character = Cast<AAliCharacter>(GetOwner());
	if (!Character)
		return;
	const FGameplayTag Cards = FGameplayTag::RequestGameplayTag(TEXT("Als.OverlayMode.Cards"));
	const bool bHasCard =
		SlotOrder.IsValidIndex(CurrentSlotIndex) && CardCopyCounts.FindRef(SlotOrder[CurrentSlotIndex]) > 0;
	const bool bAlreadyCards = Character->GetOverlayMode() == Cards;
	if (bHasCard)
		Character->SetOverlayMode(Cards);
	else if (bAlreadyCards)
		Character->SetOverlayMode(AlsOverlayModeTags::Default);
	// ALS skips unchanged tags. Re-equipping must also restore the linked card layer
	// if another action relinked it while the Cards tag remained selected.
	if (bHasCard && bAlreadyCards)
		if (UFunction* Refresh = Character->FindFunction(TEXT("RefreshOverlayLinkedAnimationLayer")))
			Character->ProcessEvent(Refresh, nullptr);
}
