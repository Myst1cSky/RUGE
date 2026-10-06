// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Rules/CRuleData.h"
#include "CRuleManager.generated.h"

class UDataTable;
class UCSanityComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRuleBroken, FName, RuleID);

// Runtime state for one rule (not a UObject, so no reflection needed)
struct FCActiveRule
{
	FCRuleData Data;
	bool bActive = false;
	bool bResolved = false;
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
	
	void StartRound(UDataTable* Table, int32 Round);
	
	// Gives every unresolved rule its final verdict. 
	// Returns true if the round had no mistakes.
	bool EndRound();
	
	// Triggers and interactable(s) call this with an event name
	void ReportEvent(FName EventTag);
	
	UPROPERTY(BlueprintAssignable, Category = "Rules")
	FOnRuleBroken OnRuleBroken;

private:	
	void Resolve(FCActiveRule& Rule, bool bDidIt);
	UCSanityComponent* GetPlayerSanity() const;

	TArray<FCActiveRule> Rules;
	float RoundTime = 0.f;
	bool bRoundRunning = false;
	bool bFlawless = true;
};
