// ABaseEnemy: Damage. Included once by ABaseEnemy.cpp; retains original definition order.

void AABaseEnemy::ApplyGrenadeBlast(FVector Origin, float Radius, float Strength)
{
	if (!HasAuthority() || Origin.ContainsNaN() || !FMath::IsFinite(Radius) || Radius <= 0.0f ||
		!FMath::IsFinite(Strength) || Strength <= 0.0f || !GetMesh())
		return;
	StartRagdolling();
	if (GetLocomotionAction() != AlsLocomotionActionTags::Ragdolling)
		return;
	// ALS suppresses accidental initial physics velocity for eight frames. A
	// deliberate grenade impulse must survive those normal ragdoll updates.
	RagdollingState.SpeedLimitFrameTimeRemaining = 0;
	GetMesh()->AddRadialImpulse(Origin, Radius, Strength, ERadialImpulseFalloff::RIF_Linear, true);
}

float AABaseEnemy::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
							  AActor* DamageCauser)
{
	FHitResult EmptyHit;
	const FHitResult* Hit = DamageEvent.IsOfType(FPointDamageEvent::ClassID)
								? &static_cast<const FPointDamageEvent&>(DamageEvent).HitInfo
								: nullptr;
	AActor* Source = DamageCauser ? DamageCauser : EventInstigator ? EventInstigator->GetPawn() : nullptr;
	if (DamageAmount > 0.0f && TryInterceptIncomingDamage(DamageAmount, Source, Hit ? *Hit : EmptyHit, Hit != nullptr))
		return 0.0f;
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

bool AABaseEnemy::TryInterceptIncomingDamage_Implementation(float Damage, AActor* Source, const FHitResult& Hit,
															bool bHasHit)
{
	return false;
}
