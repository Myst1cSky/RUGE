// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CGameModeBase.generated.h"

class UCRuleManager;
class UCSanityComponent;
class UCNarratorComponent;
class UCSubtitleWidget;
class UDataTable;
class UCEndScreenWidget;

UCLASS()
class ACGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ACGameModeBase();

	UCRuleManager* GetRuleManager() const { return RuleManager; }
	
	// Called by the player's replay key
	void ReplayRules();
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Rules")
	TObjectPtr<UCRuleManager> RuleManager;

	UPROPERTY(EditDefaultsOnly, Category = "Rules")
	TObjectPtr<UDataTable> RuleTable;
	
	UPROPERTY(VisibleAnywhere, Category = "Narrator")
	TObjectPtr<UCNarratorComponent> Narrator;

	UPROPERTY(EditDefaultsOnly, Category = "Narrator")
	TObjectPtr<UDataTable> NarratorTable;

	UPROPERTY(EditDefaultsOnly, Category = "Narrator")
	TSubclassOf<UCSubtitleWidget> SubtitleWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Hour")
	int32 TotalHours = 8;

	// Real seconds per in-game hour
	UPROPERTY(EditDefaultsOnly, Category = "Hour")
	float HourDuration = 75.f;

	UPROPERTY(EditDefaultsOnly, Category = "Hour")
	float SanityRecoveryPerHour = 5.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Hour")
	int32 CountdownSeconds = 5;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCEndScreenWidget> EndScreenWidgetClass;

	// Seconds between the ending and the screen appearing (lets the narrator speak)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float EndScreenDelay = 3.f;

private:
	void StartGame();
	void StartHour(int32 Hour);
	void EndHour();
	void GameComplete();
	void GameOver();
	void HandleNarratorIdle();
	void TickCountdown();
	void BeginRules();

	bool bWaitingForNarrator = false;
	int32 CountdownRemaining = 0;
	FTimerHandle CountdownTimer;
	
	int32 CurrentHour = 0;
	bool bGameEnded = false;
	FTimerHandle HourTimer;
	
	UFUNCTION()
	void HandleSanityDepleted();

	UFUNCTION()
	void HandleStageChanged(ESanityStage OldStage, ESanityStage NewStage);

	UFUNCTION()
	void HandleRuleAnnounced(FName RuleID, FText RuleText);

	UFUNCTION()
	void HandleRuleExpired(FName RuleID, FText RuleText);

	UFUNCTION()
	void HandleRuleBroken(FName RuleID);
	
	UCSanityComponent* GetPlayerSanity() const;
	
	void ShowEndScreen(bool bWon);
	FTimerHandle EndScreenTimer;

};
