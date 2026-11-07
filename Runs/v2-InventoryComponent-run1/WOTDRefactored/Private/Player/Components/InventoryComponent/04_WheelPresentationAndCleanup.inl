// InventoryComponent: WheelPresentationAndCleanup. Included once by InventoryComponent.cpp; retains original definition order.

void UInventory::UpdateCardWheel()
{
	const APawn* Pawn = Cast<APawn>(GetOwner());
	APlayerController* Controller = Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;
	if (!Controller || !Controller->IsLocalController())
	{
		TearDownCardWidgets();
		bWasWheelHeld = false;
		bWasInventoryKeyDown = false;
		bWasHandToggleKeyDown = false;
		return;
	}
	if (CardWheel && CardWheel->GetOwningPlayer() != Controller)
	{
		CardWheel->CloseWheel(false);
		CardWheel->RemoveFromParent();
		CardWheel = nullptr;
		bWasWheelHeld = false;
	}
	if (CardInventoryPanel && CardInventoryPanel->GetOwningPlayer() != Controller)
	{
		CardInventoryPanel->CloseInventory(false);
		CardInventoryPanel->RemoveFromParent();
		CardInventoryPanel = nullptr;
		bWasInventoryKeyDown = false;
	}
	if (!CardInventoryPanel)
	{
		CardInventoryPanel = CreateWidget<UCardInventoryPanel>(Controller, UCardInventoryPanel::StaticClass());
		if (CardInventoryPanel)
			CardInventoryPanel->AddToPlayerScreen(60);
	}
	const bool bInventoryKeyDown = Controller->IsInputKeyDown(EKeys::Tab);
	const bool bWheelHeld = Controller->IsInputKeyDown(EKeys::Gamepad_LeftShoulder);
	if (!CardWheel)
	{
		AcquireCardWheel(Controller);
	}
	UpdateInventoryPanel(Controller, bInventoryKeyDown);
	if (CardWheel)
		CardWheel->SetInventory(this);
	UpdateHandLayoutToggle(Controller);
	UpdateWheelInputAndAnimation(Controller, bWheelHeld);
	bWasWheelHeld = bWheelHeld;
	bWasInventoryKeyDown = bInventoryKeyDown;
	bWasReserveWheelHeld = bInventoryKeyDown;
}

void UInventory::TearDownCardWidgets()
{
	if (CardWheel)
	{
		CardWheel->CloseWheel(false);
		CardWheel->RemoveFromParent();
		CardWheel = nullptr;
	}
	if (CardInventoryPanel)
	{
		CardInventoryPanel->CloseInventory(false);
		CardInventoryPanel->RemoveFromParent();
		CardInventoryPanel = nullptr;
	}
}

void UInventory::AcquireCardWheel(APlayerController* Controller)
{
	// Prefer the HUD's embedded wheel so card selection has one visual owner.
	TArray<UUserWidget*> Widgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, Widgets, UCardWheel::StaticClass(), false);
	for (UUserWidget* Widget : Widgets)
	{
		if (Widget->GetOwningPlayer() == Controller && Widget->GetParent())
		{
			CardWheel = Cast<UCardWheel>(Widget);
			break;
		}
	}
	if (!CardWheel)
	{
		CardWheel = CreateWidget<UCardWheel>(Controller, UCardWheel::StaticClass());
		if (CardWheel)
		{
			CardWheel->SetVisibility(ESlateVisibility::HitTestInvisible);
			CardWheel->AddToPlayerScreen(50);
		}
	}
}

void UInventory::UpdateInventoryPanel(APlayerController* Controller, bool bInventoryKeyDown)
{
	if (CardInventoryPanel)
		CardInventoryPanel->SetInventory(this);
	if (CardInventoryPanel && bInventoryKeyDown && !bWasInventoryKeyDown)
	{
		if (!Controller->bShowMouseCursor && !Controller->IsMoveInputIgnored())
			CardInventoryPanel->OpenInventory(this);
	}
	if (CardInventoryPanel)
	{
		CardInventoryPanel->UpdateInput();
		if (!bInventoryKeyDown && bWasInventoryKeyDown)
			CardInventoryPanel->CloseInventory(true);
	}
}

void UInventory::UpdateHandLayoutToggle(APlayerController* Controller)
{
	// Use one visual owner and rising edges: holding either key never repeats the flip.
	// Pause/reserve/recycling consume the press instead of deferring a surprise toggle.
	const bool bHandToggleDown =
		Controller->IsInputKeyDown(CardHandKeyboardKey) || Controller->IsInputKeyDown(CardHandControllerKey);
	if (CardWheel && bHandToggleDown && !bWasHandToggleKeyDown && !bIsRecycling && !Controller->bShowMouseCursor &&
		!Controller->IsMoveInputIgnored() && !(CardInventoryPanel && CardInventoryPanel->IsInventoryOpen()))
	{
		CardWheel->ToggleHandLayout();
	}
	bWasHandToggleKeyDown = bHandToggleDown;
}

void UInventory::UpdateWheelInputAndAnimation(APlayerController* Controller, bool bWheelHeld)
{
	if (bWheelHeld && !bWasWheelHeld && !Controller->bShowMouseCursor && !Controller->IsMoveInputIgnored())
	{
		if (CardWheel)
			CardWheel->OpenWheel(this);
	}
	if (CardWheel)
	{
		CardWheel->AdvanceWheelAnimation(GetWorld()->GetDeltaSeconds());
		CardWheel->UpdateInput();
		if (!bWheelHeld && bWasWheelHeld)
			CardWheel->CloseWheel(true);
	}
}

void UInventory::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	TearDownCardWidgets();
	PendingRefills.Empty();
	Super::EndPlay(EndPlayReason);
}
