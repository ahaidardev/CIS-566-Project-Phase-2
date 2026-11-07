#pragma once

#include "Enemy/ABaseEnemy.h"
#include "Perception/AIPerceptionTypes.h"
#include "Suspicion/ModularSuspicionComponent.h"
#include "TrackerEnemy.generated.h"

UCLASS(Blueprintable)
class WOTDREFACTORED_API ATrackerEnemy : public AABaseEnemy
{
	GENERATED_BODY()

  public:
	UFUNCTION(BlueprintCallable, Category = "Tracker")
	bool PingTarget(FVector SearchLocation);
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tracker|Perception", meta = (ClampMin = "0.0"))
	float NoisePingCooldown = 2.0f;

  protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  private:
	void BindTrackerPerception();
	void BindTrackerSuspicion();
	void ReplayCurrentlySeenActors();
	void UnbindTrackerPerception();
	bool HasVisiblePlayer();
	void HandleNoiseLead(AActor* Source, const FVector& Location);
	bool IsSuspicious() const;
	void TryPlayerSightLead(AActor* Player, const FVector& Location);
	UFUNCTION()
	void OnTrackerSuspicionChanged(ESuspicionColorStatus Color);
	UFUNCTION()
	void OnTrackerPlayerIdentified(AActor* Player);
	UFUNCTION()
	void OnTrackerPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	UFUNCTION()
	void OnTrackerPawnNoise(APawn* Pawn, const FVector& Location, float Volume);
	TWeakObjectPtr<class UAIPerceptionComponent> TrackerPerception;
	TWeakObjectPtr<class UPawnSensingComponent> TrackerPawnSensing;
	TSet<TWeakObjectPtr<AActor>> VisiblePlayers;
	TSet<TWeakObjectPtr<AActor>> PingedVisiblePlayers;
	TWeakObjectPtr<UModularSuspicionComponent> TrackerSuspicion;
	double LastNoisePingTime = -DBL_MAX;
};
