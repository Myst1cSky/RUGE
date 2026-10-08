// Fill out your copyright notice in the Description page of Project Settings.


#include "Rules/CRuleManager.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Sanity/CSanityComponent.h"

// Sets default values for this component's properties
UCRuleManager::UCRuleManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCRuleManager::StartRound(UDataTable* Table, int32 Round)
{
	Rules.Reset();
	RoundTime = 0.f;
	bFlawless = true;
	bRoundRunning = false;
	
	if (!Table) return;
	
	Table->ForeachRow<FCRuleData>(TEXT("StartRound"),
		[this, Round](const FName& Key, const FCRuleData& Row)
		{
			if (Row.Round == Round)
			{
				FCActiveRule NewRule;
				NewRule.Data = Row;
				Rules.Add(NewRule);
			}
		});
	
	// Temporary stand-in for the narrator and handbook
	if (GEngine)
	{
		for (const FCActiveRule& R : Rules)
		{
			GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Cyan,
				FString::Printf(TEXT("RULE: %s"), *R.Data.RuleText.ToString()));
		}
	}

	bRoundRunning = true;
}

// Called every frame
void UCRuleManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!bRoundRunning) return;

	RoundTime += DeltaTime;

	for (FCActiveRule& R : Rules)
	{
		if (R.bResolved) continue;

		if (!R.bActive && RoundTime >= R.Data.WindowStart)
		{
			R.bActive = true;
		}
		if (!R.bActive) continue;

		const bool bWindowClosed = RoundTime >= R.Data.WindowStart + R.Data.WindowDuration;

		if (R.Data.bContinuous)
		{
			if (bWindowClosed)
			{
				R.bResolved = true;   // simply stops draining
			}
			else
			{
				TickContinuous(R, DeltaTime);
			}
			continue;
		}

		// Window closed with no event: the player did nothing
		if (bWindowClosed)
		{
			Resolve(R, false);
		}
	}
}

void UCRuleManager::ReportEvent(FName EventTag)
{
	if (!bRoundRunning) return;
	
	for (FCActiveRule& R : Rules)
	{
		if (!R.bResolved && R.bActive && !R.Data.bContinuous && R.Data.WatchedEvent == EventTag)
		{
			Resolve(R, true);
		}
	}
}

bool UCRuleManager::EndRound()
{
	bRoundRunning = false;
	
	for (FCActiveRule& R : Rules)
	{
		if (!R.bResolved && !R.Data.bContinuous)
		{
			Resolve(R, false);
		}
	}
	
	const bool bResult = bFlawless;
	Rules.Reset();
	return bResult;
}

void UCRuleManager::SetZoneOccupied(FName ZoneTag, bool bOccupied)
{
	if (bOccupied)
	{
		OccupiedZones.Add(ZoneTag);
	}
	else
	{
		OccupiedZones.Remove(ZoneTag);
	}
}

void UCRuleManager::Resolve(FCActiveRule& Rule, bool bDidIt)
{
	Rule.bResolved = true;

	// Action rules are followed by doing it, restraint rules by not doing it
	const bool bFollowed = (Rule.Data.Kind == ERuleKind::Action) ? bDidIt : !bDidIt;
	const bool bMistake = (bFollowed != Rule.Data.bIsTrue);

	if (!bMistake) return;

	bFlawless = false;

	if (UCSanityComponent* Sanity = GetPlayerSanity())
	{
		Sanity->Drain(Rule.Data.Penalty, Rule.Data.Reason);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red,
			FString::Printf(TEXT("RULE BROKEN: %s"), *Rule.Data.RuleID.ToString()));
	}

	OnRuleBroken.Broadcast(Rule.Data.RuleID);
}

UCSanityComponent* UCRuleManager::GetPlayerSanity() const
{
	if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		return Pawn->FindComponentByClass<UCSanityComponent>();
	}
	return nullptr;
}

void UCRuleManager::TickContinuous(FCActiveRule& Rule, float DeltaTime)
{
	const bool bInside = OccupiedZones.Contains(Rule.Data.DrainZone);
	const bool bViolating = (bInside == Rule.Data.bDrainWhenInside);

	if (!bViolating)
	{
		Rule.DrainTimer = 0.f; // back in the right place, start fresh next time
		return;
	}

	// First violation of this rule: mark the round and announce it once
	if (!Rule.bViolationAnnounced)
	{
		Rule.bViolationAnnounced = true;
		bFlawless = false;

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red,
				FString::Printf(TEXT("RULE BROKEN: %s"), *Rule.Data.RuleID.ToString()));
		}
		OnRuleBroken.Broadcast(Rule.Data.RuleID);
	}

	// One drain per full second, independent of frame rate
	Rule.DrainTimer += DeltaTime;
	while (Rule.DrainTimer >= 1.f)
	{
		Rule.DrainTimer -= 1.f;

		if (UCSanityComponent* Sanity = GetPlayerSanity())
		{
			Sanity->Drain(Rule.Data.DrainPerSecond, Rule.Data.Reason);
		}
	}
}

