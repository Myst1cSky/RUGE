// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Sound/SoundBase.h"
#include "CNarratorData.generated.h"

UENUM(BlueprintType)
enum class ENarratorPriority : uint8
{
	Low,	// dropped if the narrator is already busy
	Normal, // queued
	High,	// clears the queue and plays immediately
};

// The row name in the DataTable is the line ID
USTRUCT(BlueprintType)
struct FCNarratorLine : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Subtitle;

	// Leave empty for subtitle-only
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundBase> Sound;

	// Silence after the line, in seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PauseAfter = 1.f;
};
