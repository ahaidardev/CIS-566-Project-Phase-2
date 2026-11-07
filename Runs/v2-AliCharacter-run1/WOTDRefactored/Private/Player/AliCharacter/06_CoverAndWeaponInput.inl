// AliCharacter: CoverAndWeaponInput. Included once by AliCharacter.cpp; retains original definition order.

// ReSharper disable once CppMemberFunctionMayBeConst
void AAliCharacter::Input_OnSwitchShoulder()
{
	Camera->SetRightShoulder(!Camera->IsRightShoulder());
}

void AAliCharacter::Input_OnCoverMove(const FInputActionValue& ActionValue)
{
	if (IsValid(CoverComponent) && CoverComponent->GetCoverBool())
	{
		const float X = ActionValue.Get<FVector2D>().X;
		CoverComponent->TryMove(X);
	}
}

void AAliCharacter::Input_OnCoverMoveEnd()
{
	if (IsValid(CoverComponent))
		CoverComponent->TryMove(0.f);
}

void AAliCharacter::Input_OnParkour()
{
	//CoverComponent->TryToggleCover();
}

void AAliCharacter::Input_OnShoot()
{
	//get dt_carditems
	// if (IsDesiredAiming())
	// {
	//
	// }
	// else
	// {
	//
	// }
	// if(!InventoryComponent->MyCards.IsEmpty())
	// InventoryComponent->RemoveCardFromInventory(InventoryComponent->SelectedCard);
	// Note: Remove this if its not going to be used
}
