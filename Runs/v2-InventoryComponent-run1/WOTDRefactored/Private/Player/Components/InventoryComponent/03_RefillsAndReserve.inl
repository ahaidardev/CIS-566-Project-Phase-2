// InventoryComponent: RefillsAndReserve. Included once by InventoryComponent.cpp; retains original definition order.

float UInventory::GetRefillRemaining(TSubclassOf<UGSCGameplayAbility> AbilityClass) const
{
	const FPendingCardRefill* Pending = PendingRefills.Find(AbilityClass);
	return Pending ? FMath::Max(0.f, Pending->RemainingTime) : 0.f;
}

void UInventory::UpdateAutoRefills(float DeltaTime)
{
	if (!FMath::IsFinite(DeltaTime) || DeltaTime <= 0.f)
		return;
	TArray<TSubclassOf<UGSCGameplayAbility>> Keys;
	PendingRefills.GetKeys(Keys);
	for (const auto& Key : Keys)
	{
		FPendingCardRefill* Pending = PendingRefills.Find(Key);
		if (!Pending)
			continue;
		Pending->RemainingTime -= DeltaTime;
		Pending = GrantDueRefillCharges(Pending, Key);
		if (Pending && Pending->MissingCharges == 0)
			PendingRefills.Remove(Key);
	}
}

FPendingCardRefill* UInventory::GrantDueRefillCharges(FPendingCardRefill* Pending,
													  const TSubclassOf<UGSCGameplayAbility>& Key)
{
	while (Pending && Pending->RemainingTime <= 0.f && Pending->MissingCharges > 0)
	{
		const FCardItem Card = Pending->Card;
		if (!CanAddCard(Card))
			break;
		Pending->RemainingTime +=
			FMath::IsFinite(Card.RefillInterval) ? FMath::Max(0.1f, Card.RefillInterval) : 10.f;
		AddCardToInventory_Implementation(Card);
		Pending = PendingRefills.Find(Key);
	}
	return Pending;
}

TArray<FCardItem> UInventory::GetWheelCards() const
{
	TArray<FCardItem> Cards;
	for (const auto& Key : SlotOrder)
	{
		if (const FCardItem* Card = CardData.Find(Key))
			Cards.Add(*Card);
	}
	TArray<TSubclassOf<UGSCGameplayAbility>> PendingKeys;
	PendingRefills.GetKeys(PendingKeys);
	PendingKeys.Sort([](const TSubclassOf<UGSCGameplayAbility>& A, const TSubclassOf<UGSCGameplayAbility>& B)
					 { return GetNameSafe(A.Get()) < GetNameSafe(B.Get()); });
	for (const auto& Key : PendingKeys)
	{
		if (!SlotOrder.Contains(Key))
			Cards.Add(PendingRefills[Key].Card);
	}
	return Cards;
}

TArray<FCardItem> UInventory::GetReserveCards() const
{
	TArray<TSubclassOf<UGSCGameplayAbility>> Keys;
	ReserveCardCounts.GetKeys(Keys);
	Keys.Sort([](const auto& A, const auto& B) { return GetNameSafe(A.Get()) < GetNameSafe(B.Get()); });
	TArray<FCardItem> Cards;
	for (const auto& Key : Keys)
	{
		if (const FCardItem* Card = ReserveCardData.Find(Key))
			Cards.Add(*Card);
	}
	return Cards;
}

bool UInventory::SwapSelectedCardWithReserve(TSubclassOf<UGSCGameplayAbility> ReserveClass)
{
	if (!ReserveClass || !SlotOrder.IsValidIndex(CurrentSlotIndex) || !ReserveCardCounts.Contains(ReserveClass) ||
		!ReserveCardData.Contains(ReserveClass))
		return false;
	const TSubclassOf<UGSCGameplayAbility> EquippedClass = SlotOrder[CurrentSlotIndex];
	if (EquippedClass == ReserveClass)
		return false;

	const int32 ReserveCount = ReserveCardCounts[ReserveClass];
	const FCardItem ReserveData = ReserveCardData[ReserveClass];
	ReserveCardCounts.Remove(ReserveClass);
	ReserveCardData.Remove(ReserveClass);
	MoveEquippedStackToReserve(EquippedClass);
	EquipReserveStack(ReserveClass, ReserveCount, ReserveData);
	OnCardSelected.Broadcast(CurrentSlotIndex);
	return true;
}

void UInventory::MoveEquippedStackToReserve(const TSubclassOf<UGSCGameplayAbility>& EquippedClass)
{
	ReserveCardCounts.Add(EquippedClass, CardCopyCounts.FindRef(EquippedClass));
	ReserveCardData.Add(EquippedClass, CardData.FindRef(EquippedClass));
	CardCopyCounts.Remove(EquippedClass);
	CardData.Remove(EquippedClass);
}

void UInventory::EquipReserveStack(const TSubclassOf<UGSCGameplayAbility>& ReserveClass, int32 ReserveCount,
								   const FCardItem& ReserveData)
{
	CardCopyCounts.Add(ReserveClass, ReserveCount);
	CardData.Add(ReserveClass, ReserveData);
	SlotOrder[CurrentSlotIndex] = ReserveClass;
}

int32 UInventory::GetReserveCardCount(TSubclassOf<UGSCGameplayAbility> AbilityClass) const
{
	return ReserveCardCounts.FindRef(AbilityClass);
}
