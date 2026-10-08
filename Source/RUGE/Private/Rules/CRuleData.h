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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Round = 1;
	
	// If true, following the instruction is correct.
	// If false, following it is the mistake.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsTrue = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERuleKind Kind = ERuleKind::Restraint;
	
	// The event this rule watches, e.g. Zone.StartRoom.Exit or Door.Open
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName WatchedEvent;
	
	// Seconds after round start when the rule becomes active
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WindowStart = 0.f;
	
	// How long the rule stays active
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WindowDuration = 30.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Penalty = 10.f;
	
	// Label passed to Drain (useful for narrator reactions later)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Reason;
	
	//---------Continuous Drain Rules----------
	
	// If true, this rule drains sanity over time instead of one-time on a verdict
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bContinuous = false;
	
	// The zone to watch, e.g. Zone.StartRoom (matches the Zone Tag on the placed trigger)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DrainZone;
	
	// true = drain while the player is inside the zone, false = drain while outside it
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDrainWhenInside = false;
	
	// Sanity lost each second of violation
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DrainPerSecond = 1.f;
};
