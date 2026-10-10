// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CRuleData.generated.h"

UENUM(BlueprintType)
enum class ERuleKind : uint8
{
	Action, // Do X
	Restraint // Don't do X
};

USTRUCT(BlueprintType)
struct FCRuleData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RuleID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText RuleText;

	// First hour the rule applies (1 to 8)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StartHour = 1;

	// Last hour the rule applies. 0 = never expires.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 EndHour = 0;

	// Seconds into each hour when the rule becomes active
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WindowStart = 0.f;

	// How long it stays active each hour
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WindowDuration = 75.f;

	// Label shown in the sanity debug message
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Reason;

	// ---- One-time rules (Continuous unticked), judged once per hour ----

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsTrue = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERuleKind Kind = ERuleKind::Restraint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName WatchedEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Penalty = 10.f;

	// ---- Continuous rules (Continuous ticked) ----

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bContinuous = false;

	// Must match the Zone Tag on the placed zone exactly
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DrainZone;

	// Ticked = drain while inside the zone. Unticked = drain while outside it.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDrainWhenInside = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DrainPerSecond = 1.f;
};
