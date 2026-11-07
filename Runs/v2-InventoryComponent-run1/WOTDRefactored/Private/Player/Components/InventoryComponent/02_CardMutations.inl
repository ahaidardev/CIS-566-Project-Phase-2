// InventoryComponent: CardMutations. Included once by InventoryComponent.cpp; retains original definition order.

FCardItem UInventory::AddCardToInventory_Implementation(const FCardItem& Card)
{
	if (!CanAddCard(Card))
	{
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Card rejected!"));
		return {};
	}

	if (FPendingCardRefill* Pending = PendingRefills.Find(Card.AbilityClass))
	{
		if (--Pending->MissingCharges <= 0)
			PendingRefills.Remove(Card.AbilityClass);
	}

	if (CardCopyCounts.Contains(Card.AbilityClass))
	{
		// Stack already exists, just increment the copy count
		CardCopyCounts[Card.AbilityClass]++;
	}
	else if (ReserveCardCounts.Contains(Card.AbilityClass))
	{
		ReserveCardCounts[Card.AbilityClass]++;
	}
	else if (SlotOrder.Num() >= 5)
	{
		ReserveCardCounts.Add(Card.AbilityClass, 1);
		ReserveCardData.Add(Card.AbilityClass, Card);
	}
	else
	{
		// New unique card type, add to map and slot order
		CardCopyCounts.Add(Card.AbilityClass, 1);
		CardData.Add(Card.AbilityClass, Card);
		SlotOrder.Add(Card.AbilityClass);

		// Auto select if this is the first card
		if (SlotOrder.Num() == 1)
		{
			CurrentSlotIndex = 0;
		}
	}

	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple,
	//     FString::Printf(TEXT("CardAdded: CurrentSlotIndex = %d, SlotOrder.Num() = %d"),
	//     CurrentSlotIndex, SlotOrder.Num()));
	OnCardAdded.Broadcast(Card);
	OnCardSelected.Broadcast(CurrentSlotIndex);

	// #if UE_BUILD_DEBUG
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
	// FString::Printf(TEXT("%s added to inventory!"), *Card.CardName.ToString()));
	// #endif

	return Card;
}

FCardItem UInventory::RemoveSelectedCard()
{
	if (SlotOrder.IsEmpty())
		return {};
	if (!SlotOrder.IsValidIndex(CurrentSlotIndex))
		return {};

	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,
	// FString::Printf(TEXT("CurrentSlotIndex = %d, SlotOrder.Num() = %d"),
	// CurrentSlotIndex, SlotOrder.Num()));

	TSubclassOf<UGSCGameplayAbility> SelectedClass = SlotOrder[CurrentSlotIndex];

	if (!CardCopyCounts.Contains(SelectedClass) || CardCopyCounts[SelectedClass] <= 0 ||
		!CardData.Contains(SelectedClass))
		return {};

	// Decrement copy count
	CardCopyCounts[SelectedClass]--;

	// Get the card data to return and broadcast
	FCardItem RemovedCard = CardData[SelectedClass];

	if (RemovedCard.RefillType == ECardRefillType::Automatic && !bIsRecycling)
	{
		FPendingCardRefill& Pending = PendingRefills.FindOrAdd(SelectedClass);
		if (Pending.MissingCharges == 0)
		{
			Pending.Card = RemovedCard;
			Pending.RemainingTime =
				FMath::IsFinite(RemovedCard.RefillInterval) ? FMath::Max(0.1f, RemovedCard.RefillInterval) : 10.f;
		}
		++Pending.MissingCharges;
	}

	// If no copies remain, remove the slot entirely
	if (CardCopyCounts[SelectedClass] <= 0)
	{
		CardCopyCounts.Remove(SelectedClass);
		CardData.Remove(SelectedClass);
		SlotOrder.Remove(SelectedClass);

		// Adjust selection index
		if (SlotOrder.IsEmpty())
		{
			CurrentSlotIndex = 0;
		}
		else if (CurrentSlotIndex >= SlotOrder.Num())
		{
			CurrentSlotIndex = SlotOrder.Num() - 1;
		}
	}

	OnCardRemoved.Broadcast(RemovedCard);
	OnCardSelected.Broadcast(CurrentSlotIndex);
	return RemovedCard;
}
