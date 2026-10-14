// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Narrator/CNarratorData.h"
#include "CNarratorComponent.generated.h"

class UDataTable;
class UAudioComponent;
class UCSubtitleWidget;

struct FCQueuedLine
{
	FText Subtitle;
	TSoftObjectPtr<USoundBase> Sound;
	float PauseAfter = 1.f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCNarratorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCNarratorComponent();
	
	// Creates the subtitle widget and stores the line table
	void Init(UDataTable* InTable, TSubclassOf<UCSubtitleWidget> WidgetClass);

	// Looks up LineID in the table. If missing, uses Fallback as a subtitle-only line.
	// If both are missing, nothing happens.
	void SayLine(FName LineID, const FText& Fallback, ENarratorPriority Priority = ENarratorPriority::Normal);

	void SayHour(int32 Hour, const FText& Fallback);
	void SayRule(FName RuleID, const FText& RuleText);
	void SayReaction(FName RuleID);

	void SetVoicePitch(float Pitch) { VoicePitch = Pitch; }
	bool IsBusy() const { return bBusy; }
	void StopAll();
	
	FSimpleMulticastDelegate OnQueueEmpty;
	
protected:
	// Reading time per character for lines with no sound
	UPROPERTY(EditAnywhere, Category = "Narrator")
	float SecondsPerCharacter = 0.06f;

	UPROPERTY(EditAnywhere, Category = "Narrator")
	float MinTextDuration = 2.f;

	// Minimum seconds between mistake reactions
	UPROPERTY(EditAnywhere, Category = "Narrator")
	float ReactionCooldown = 6.f;

	// How many Reaction_Generic_N rows exist in the table
	UPROPERTY(EditAnywhere, Category = "Narrator")
	int32 GenericReactionCount = 3;

private:
	void Enqueue(const FCQueuedLine& Line, ENarratorPriority Priority, bool bToFront = false);
	void PlayNext();
	void FinishLine(float PauseAfter);
	bool FindLine(FName LineID, FCQueuedLine& OutLine) const;

	UPROPERTY()
	TObjectPtr<UDataTable> LineTable;

	UPROPERTY()
	TObjectPtr<UCSubtitleWidget> SubtitleWidget;

	UPROPERTY()
	TObjectPtr<UAudioComponent> ActiveSound;

	TArray<FCQueuedLine> Queue;
	FTimerHandle NarratorTimer;
	bool bBusy = false;
	float VoicePitch = 1.f;
	float LastReactionTime = -100.f;
	int32 LastGenericIndex = 0;
};
