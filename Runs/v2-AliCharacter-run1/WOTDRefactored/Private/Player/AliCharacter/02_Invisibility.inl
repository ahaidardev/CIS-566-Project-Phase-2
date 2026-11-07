// AliCharacter: Invisibility. Included once by AliCharacter.cpp; retains original definition order.

void AAliCharacter::InitializeDynamicMaterials()
{
	TInlineComponentArray<UMeshComponent*> Meshes(this);
	for (UMeshComponent* MeshComponent : Meshes)
	{
		if (!IsValid(MeshComponent))
			continue;
		for (int32 Slot = 0; Slot < MeshComponent->GetNumMaterials(); ++Slot)
		{
			UMaterialInterface* Material = MeshComponent->GetMaterial(Slot);
			if (!IsValid(Material))
				continue;
			UMaterialInterface* Source = Material;
			while (const UMaterialInstanceDynamic* DynamicSource = Cast<UMaterialInstanceDynamic>(Source))
			{
				Source = DynamicSource->Parent;
			}
			const TObjectPtr<UMaterialInterface>* Variant = InvisibilityMaterialVariants.Find(Source);
			UMaterialInstanceDynamic* Dynamic = Cast<UMaterialInstanceDynamic>(Material);
			if (Variant && IsValid(Variant->Get()))
			{
				UMaterialInstanceDynamic* Replacement = UMaterialInstanceDynamic::Create(Variant->Get(), this);
				if (Replacement && Dynamic)
				{
					Replacement->CopyParameterOverrides(Dynamic);
				}
				Dynamic = Replacement;
			}
			else if (!Dynamic || !Dynamic->IsIn(this))
			{
				Dynamic = UMaterialInstanceDynamic::Create(Material, this);
			}
			if (!IsValid(Dynamic))
				continue;
			MeshComponent->SetMaterial(Slot, Dynamic);
			AliDynMatRef.AddUnique(Dynamic);
			if (!VisibleMaterialAppearance.Contains(Dynamic))
			{
				float Appearance = 0.0f;
				if (Dynamic->GetScalarParameterValue(FMaterialParameterInfo(TEXT("Appearance")), Appearance))
				{
					VisibleMaterialAppearance.Add(Dynamic, Appearance);
				}
			}
		}
	}
	ApplyInvisibilityAppearance();
}

void AAliCharacter::BecomeInvisible()
{
	InitializeDynamicMaterials();
	StartInvisibilityTransition(1.0f);
}

void AAliCharacter::BecomeVisible()
{
	StartInvisibilityTransition(0.0f);
}

void AAliCharacter::StartInvisibilityTransition(const float TargetAlpha)
{
	if (GetWorld())
	{
		UpdateInvisibilityTransition();
		GetWorldTimerManager().ClearTimer(InvisibilityTransitionTimer);
	}
	InvisibilityStartAlpha = InvisibilityAlpha;
	InvisibilityTargetAlpha = TargetAlpha;
	if (!GetWorld() || InvisibilityTransitionDuration <= 0.0f || FMath::IsNearlyEqual(InvisibilityAlpha, TargetAlpha))
	{
		InvisibilityAlpha = TargetAlpha;
		ApplyInvisibilityAppearance();
		return;
	}
	InvisibilityTransitionStartTime = GetWorld()->GetTimeSeconds();
	GetWorldTimerManager().SetTimer(InvisibilityTransitionTimer, this, &AAliCharacter::UpdateInvisibilityTransition,
									1.0f / 60.0f, true);
}

void AAliCharacter::UpdateInvisibilityTransition()
{
	if (!GetWorld() || !GetWorldTimerManager().IsTimerActive(InvisibilityTransitionTimer))
		return;
	const float Progress =
		FMath::Clamp(static_cast<float>((GetWorld()->GetTimeSeconds() - InvisibilityTransitionStartTime) /
										FMath::Max(InvisibilityTransitionDuration, UE_SMALL_NUMBER)),
					 0.0f, 1.0f);
	InvisibilityAlpha = FMath::Lerp(InvisibilityStartAlpha, InvisibilityTargetAlpha, Progress);
	ApplyInvisibilityAppearance();
	if (Progress >= 1.0f)
		GetWorldTimerManager().ClearTimer(InvisibilityTransitionTimer);
}

void AAliCharacter::ApplyInvisibilityAppearance()
{
	// Strand visibility ignores material opacity masks; fade their physical width instead.
	TInlineComponentArray<UGroomComponent*> Grooms(this);
	for (UGroomComponent* Groom : Grooms)
	{
		if (!IsValid(Groom) || !IsValid(Groom->GroomAsset))
			continue;
		if (!VisibleGroomWidths.Contains(Groom))
		{
			TArray<FInvisibilityHairWidth> Widths;
			const auto& Rendering = Groom->GroomAsset->GetHairGroupsRendering();
			const auto& PlatformData = Groom->GroomAsset->GetHairGroupsPlatformData();
			for (int32 Index = 0; Index < Groom->GroomGroupsDesc.Num(); ++Index)
			{
				const FHairGroupDesc& Desc = Groom->GroomGroupsDesc[Index];
				float Width = Desc.HairWidth;
				if (!Desc.HairWidth_Override && Rendering.IsValidIndex(Index) && PlatformData.IsValidIndex(Index))
				{
					const auto& Geometry = Rendering[Index].GeometrySettings;
					Width = Geometry.HairWidth_Override ? Geometry.HairWidth
														: PlatformData[Index].Strands.BulkData.Header.MaxRadius * 2.0f;
				}
				Widths.Add({Desc.HairWidth, Width, Desc.HairWidth_Override});
			}
			VisibleGroomWidths.Add(Groom, MoveTemp(Widths));
		}
		const auto& Widths = VisibleGroomWidths.FindChecked(Groom);
		for (int32 Index = 0; Index < FMath::Min(Widths.Num(), Groom->GroomGroupsDesc.Num()); ++Index)
		{
			FHairGroupDesc& Desc = Groom->GroomGroupsDesc[Index];
			Desc.HairWidth = InvisibilityAlpha > 0.0f ? Widths[Index].EffectiveWidth * (1.0f - InvisibilityAlpha)
													  : Widths[Index].OriginalWidth;
			Desc.HairWidth_Override = InvisibilityAlpha > 0.0f || Widths[Index].bOriginalOverride;
		}
		Groom->UpdateHairGroupsDescAndInvalidateRenderState(false);
	}
	// Keep meshes visible to the renderer so the post-process effect survives the dissolve.
	// MF_Appearance_1 uses zero for a complete surface and two for a fully dissolved surface.
	for (const auto& Entry : VisibleMaterialAppearance)
	{
		if (UMaterialInstanceDynamic* Material = Entry.Key.Get())
		{
			Material->SetScalarParameterValue(TEXT("Appearance"),
											  FMath::Lerp(Entry.Value, InvisibleAppearance, InvisibilityAlpha));
		}
	}
}
