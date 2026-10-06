// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CGameModeBase.generated.h"

class UCRuleManager;
class UCSanityComponent;
class UDataTable;

UCLASS()
class ACGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ACGameModeBase();

	UCRuleManager* GetRuleManager() const { return RuleManager; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Rules")
	TObjectPtr<UCRuleManager> RuleManager;

	UPROPERTY(EditDefaultsOnly, Category = "Rules")
	TObjectPtr<UDataTable> RuleTable;

	UPROPERTY(EditDefaultsOnly, Category = "Rounds")
	float RoundDuration = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Rounds")
	float BreakDuration = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Rounds")
	int32 MaxRounds = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Rounds")
	float SanityRecoveryPerRound = 5.f;

private:
	void StartRound(int32 Round);
	void EndRound();
	UCSanityComponent* GetPlayerSanity() const;

	int32 CurrentRound = 0;
	FTimerHandle RoundTimer;
};
