// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Rules/CRuleData.h"
#include "CRuleManager.generated.h"

class UDataTable;
class UCSanityComponent;
class ACZoneTrigger;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRuleBroken, FName, RuleID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRuleAnnounced, FName, RuleID, FText, RuleText);

// Runtime state for one rule (not a UObject, so no reflection needed)
struct FCActiveRule
{
	FCRuleData Data;
	bool bActive = false;
	bool bResolved = false;
	float DrainTimer = 0.f;
	bool bViolationAnnounced = false;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCRuleManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCRuleManager();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// Reads every rule from the table once, at the start of the game
	void LoadRules(UDataTable* Table);

	// Resets per-hour state and announces new and expired rules
	void StartHour(int32 Hour);

	// Gives one-time rules their final verdict. Returns true if the hour had no mistakes.
	bool EndHour();

	// Stops all rule checking when game over or game complete
	void StopRules();
	
	// Re-announces every rule that applies this hour for the replay key
	void AnnounceActiveRules();
	
	// Called by zones and interactables
	void ReportEvent(FName EventTag);

	// Called by each zone when the game starts
	void RegisterZone(FName ZoneTag, ACZoneTrigger* Zone);
	
	// Rules start being checked after the countdown
	void BeginEnforcement();

	UPROPERTY(BlueprintAssignable, Category = "Rules")
	FOnRuleBroken OnRuleBroken;
	
	// A rule just started applying or is being replayed
	UPROPERTY(BlueprintAssignable, Category = "Rules")
	FOnRuleAnnounced OnRuleAnnounced;

	// A rule just stopped applying
	UPROPERTY(BlueprintAssignable, Category = "Rules")
	FOnRuleAnnounced OnRuleExpired;

private:	
	bool IsInHour(const FCRuleData& Data, int32 Hour) const;
	void Resolve(FCActiveRule& Rule, bool bDidIt);
	void TickContinuous(FCActiveRule& Rule, float DeltaTime);
	UCSanityComponent* GetPlayerSanity() const;

	TArray<FCActiveRule> Rules;
	TMap<FName, TWeakObjectPtr<ACZoneTrigger>> Zones;
	int32 CurrentHour = 0;
	float HourTime = 0.f;
	bool bHourRunning = false;
	bool bFlawless = true;
};
