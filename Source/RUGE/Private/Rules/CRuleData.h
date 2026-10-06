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
};
