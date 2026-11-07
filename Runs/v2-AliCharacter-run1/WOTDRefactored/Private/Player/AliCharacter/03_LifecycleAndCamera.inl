// AliCharacter: LifecycleAndCamera. Included once by AliCharacter.cpp; retains original definition order.

void AAliCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetWorld())
		GetWorldTimerManager().ClearTimer(InvisibilityTransitionTimer);
	InvisibilityAlpha = 0.0f;
	ApplyInvisibilityAppearance();
	VisibleMaterialAppearance.Empty();
	AliDynMatRef.Empty();
	Super::EndPlay(EndPlayReason);
}

void AAliCharacter::CalcCamera(const float DeltaTime, FMinimalViewInfo& ViewInfo)
{
	if (Camera->IsActive())
	{
		Camera->GetViewInfo(ViewInfo);

		if (IsValid(CoverComponent))
		{
			// While in cover the ALS view is blended out in favour of the cover camera. ALS builds
			// its pivot and collision trace start from mesh sockets, and the crouched cover pose
			// turns the mesh against the wall — which swings those sockets through the wall and
			// makes the ALS collision sweep slam the camera in. The cover camera is capsule-based
			// and immune to that. See FCoverCameraSettings.
			FMinimalViewInfo CoverView;
			float CoverAlpha;
			if (CoverComponent->GetCoverCameraView(CoverView, CoverAlpha))
			{
				ViewInfo.Location = FMath::Lerp(ViewInfo.Location, CoverView.Location, CoverAlpha);

				// Slerp rather than a component lerp — a raw FRotator blend takes the long way
				// round whenever the two yaws straddle the ±180 wrap, which is exactly where a
				// wall-facing anchor tends to sit.
				ViewInfo.Rotation =
					FQuat::Slerp(ViewInfo.Rotation.Quaternion(), CoverView.Rotation.Quaternion(), CoverAlpha).Rotator();
			}

			// Peek and edge offsets stack on top of whichever base won the blend above.
			FVector PeekOffset;
			float PeekFOV;
			if (CoverComponent->GetPeekCameraContribution(PeekOffset, PeekFOV))
			{
				ViewInfo.Location += PeekOffset;
				ViewInfo.FOV += PeekFOV;
			}

			FVector EdgeOffset;
			if (CoverComponent->GetEdgeCameraContribution(EdgeOffset))
			{
				ViewInfo.Location += EdgeOffset;
			}
		}
		return;
	}

	Super::CalcCamera(DeltaTime, ViewInfo);
}
