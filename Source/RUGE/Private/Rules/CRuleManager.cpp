// Fill out your copyright notice in the Description page of Project Settings.


#include "Rules/CRuleManager.h"
#include "Rules/CZoneTrigger.h"
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

void UCRuleManager::RegisterZone(FName ZoneTag, ACZoneTrigger* Zone)
{
    Zones.Add(ZoneTag, Zone);
}

void UCRuleManager::LoadRules(UDataTable* Table)
{
    Rules.Reset();
    if (!Table) return;

    Table->ForeachRow<FCRuleData>(TEXT("LoadRules"),
        [this](const FName& Key, const FCRuleData& Row)
        {
            FCActiveRule NewRule;
            NewRule.Data = Row;
            Rules.Add(NewRule);
        });
}

bool UCRuleManager::IsInHour(const FCRuleData& Data, int32 Hour) const
{
    if (Hour < Data.StartHour) return false;
    if (Data.EndHour > 0 && Hour > Data.EndHour) return false;
    return true;
}

void UCRuleManager::StartHour(int32 Hour)
{
    CurrentHour = Hour;
    HourTime = 0.f;
    bFlawless = true;
    bHourRunning = true;

    for (FCActiveRule& R : Rules)
    {
        // Fresh state every hour
        R.bActive = false;
        R.bResolved = false;
        R.DrainTimer = 0.f;
        R.bViolationAnnounced = false;

        if (!GEngine) continue;

        // Temporary stand-in for the narrator
        if (R.Data.StartHour == Hour)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan,
                FString::Printf(TEXT("NEW RULE: %s"), *R.Data.RuleText.ToString()));
        }
        else if (R.Data.EndHour > 0 && R.Data.EndHour == Hour - 1)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Orange,
                FString::Printf(TEXT("NO LONGER APPLIES: %s"), *R.Data.RuleText.ToString()));
        }
    }
}

void UCRuleManager::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bHourRunning) return;

    HourTime += DeltaTime;

    for (FCActiveRule& R : Rules)
    {
        if (R.bResolved || !IsInHour(R.Data, CurrentHour)) continue;

        if (!R.bActive && HourTime >= R.Data.WindowStart)
        {
            R.bActive = true;
        }
        if (!R.bActive) continue;

        const bool bWindowClosed = HourTime >= R.Data.WindowStart + R.Data.WindowDuration;

        if (R.Data.bContinuous)
        {
            if (bWindowClosed)
            {
                R.bResolved = true;   // stops draining until next hour
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
    if (!bHourRunning) return;

    for (FCActiveRule& R : Rules)
    {
        if (!R.bResolved && R.bActive && !R.Data.bContinuous
            && IsInHour(R.Data, CurrentHour) && R.Data.WatchedEvent == EventTag)
        {
            Resolve(R, true);
        }
    }
}

bool UCRuleManager::EndHour()
{
    bHourRunning = false;

    for (FCActiveRule& R : Rules)
    {
        if (!R.bResolved && !R.Data.bContinuous && IsInHour(R.Data, CurrentHour))
        {
            Resolve(R, false);
        }
    }

    return bFlawless;
}

void UCRuleManager::StopRules()
{
    bHourRunning = false;
}

void UCRuleManager::Resolve(FCActiveRule& Rule, bool bDidIt)
{
    Rule.bResolved = true;

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

void UCRuleManager::TickContinuous(FCActiveRule& Rule, float DeltaTime)
{
    const TWeakObjectPtr<ACZoneTrigger>* Found = Zones.Find(Rule.Data.DrainZone);
    if (!Found || !Found->IsValid())
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(static_cast<uint64>(GetTypeHash(Rule.Data.RuleID)), 5.f, FColor::Orange,
                FString::Printf(TEXT("Rule %s: no zone found with tag '%s'"),
                    *Rule.Data.RuleID.ToString(), *Rule.Data.DrainZone.ToString()));
        }
        return;
    }

    const bool bInside = Found->Get()->IsPlayerInside();
    const bool bViolating = (bInside == Rule.Data.bDrainWhenInside);

    if (!bViolating)
    {
        Rule.DrainTimer = 0.f;
        return;
    }

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

UCSanityComponent* UCRuleManager::GetPlayerSanity() const
{
    if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0))
    {
        return Pawn->FindComponentByClass<UCSanityComponent>();
    }
    return nullptr;
}

