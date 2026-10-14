// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameModeBase.h"
#include "Rules/CRuleManager.h"
#include "Narrator/CNarratorComponent.h"
#include "Sanity/CSanityComponent.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

namespace
{
	// Hour 1 = 12:00 AM, Hour 2 = 1:00 AM, and so on
	FString HourLabel(int32 Hour)
	{
		const int32 H = (Hour - 1) % 12;
		return (H == 0) ? TEXT("12:00 AM") : FString::Printf(TEXT("%d:00 AM"), H);
	}
}

ACGameModeBase::ACGameModeBase()
{
	RuleManager = CreateDefaultSubobject<UCRuleManager>(TEXT("RuleManager"));
    Narrator = CreateDefaultSubobject<UCNarratorComponent>(TEXT("Narrator"));
}

void ACGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    // Short pause so the player can get oriented before Hour 1
    FTimerHandle StartTimer;
    GetWorldTimerManager().SetTimer(StartTimer, this, &ACGameModeBase::StartGame, 3.f, false);
}

void ACGameModeBase::StartGame()
{
    bGameEnded = false;

    if (UCSanityComponent* Sanity = GetPlayerSanity())
    {
        Sanity->OnSanityDepleted.AddDynamic(this, &ACGameModeBase::HandleSanityDepleted);
        Sanity->OnStageChanged.AddDynamic(this, &ACGameModeBase::HandleStageChanged);
    }
    
    RuleManager->OnRuleAnnounced.AddDynamic(this, &ACGameModeBase::HandleRuleAnnounced);
    RuleManager->OnRuleExpired.AddDynamic(this, &ACGameModeBase::HandleRuleExpired);
    RuleManager->OnRuleBroken.AddDynamic(this, &ACGameModeBase::HandleRuleBroken);

    Narrator->Init(NarratorTable, SubtitleWidgetClass);
    Narrator->OnQueueEmpty.AddUObject(this, &ACGameModeBase::HandleNarratorIdle);
    
    RuleManager->LoadRules(RuleTable);
    StartHour(1);
}

void ACGameModeBase::StartHour(int32 Hour)
{
    if (bGameEnded) return;

    CurrentHour = Hour;

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
            FString::Printf(TEXT("=== HOUR %d  (%s) ==="), Hour, *HourLabel(Hour)));
    }

    // The hour line is queued first, so the new rules follow it
    Narrator->SayHour(Hour,
        FText::FromString(FString::Printf(TEXT("It is now %s."), *HourLabel(Hour))));
    
    RuleManager->StartHour(Hour);
    
    Narrator->SayLine(TEXT("Countdown_Intro"),
       FText::FromString(TEXT("The rules for this hour will apply in...")));

    // The countdown begins when the narrator finishes the queue
    bWaitingForNarrator = true;
}

void ACGameModeBase::EndHour()
{
    if (bGameEnded) return;

    const bool bFlawless = RuleManager->EndHour();

    if (UCSanityComponent* Sanity = GetPlayerSanity())
    {
        Sanity->Restore(SanityRecoveryPerHour);
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Green,
            FString::Printf(TEXT("Hour %d survived (%s)"), CurrentHour,
                bFlawless ? TEXT("flawless") : TEXT("mistakes made")));
    }

    if (CurrentHour < TotalHours)
    {
        StartHour(CurrentHour + 1);
    }
    else
    {
        GameComplete();
    }
}

void ACGameModeBase::GameComplete()
{
    bGameEnded = true;
    RuleManager->StopRules();

    Narrator->SayLine(TEXT("GameComplete"),
       FText::FromString(TEXT("You made it. It is morning.")), ENarratorPriority::High);
    
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green,
            FString::Printf(TEXT("%s - YOU SURVIVED THE NIGHT"), *HourLabel(TotalHours + 1)));
    }
}

void ACGameModeBase::HandleSanityDepleted()
{
    GameOver();
}

void ACGameModeBase::GameOver()
{
    if (bGameEnded) return;

    bGameEnded = true;
    GetWorldTimerManager().ClearTimer(HourTimer);
    GetWorldTimerManager().ClearTimer(CountdownTimer);
    bWaitingForNarrator = false;
    RuleManager->StopRules();

    Narrator->SayLine(TEXT("GameOver"),
        FText::FromString(TEXT("I tried to warn you.")), ENarratorPriority::High);
    
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red,
            FString::Printf(TEXT("GAME OVER - You lost your mind at Hour %d"), CurrentHour));
    }
}

void ACGameModeBase::HandleNarratorIdle()
{
    if (!bWaitingForNarrator || bGameEnded) return;

    bWaitingForNarrator = false;
    CountdownRemaining = CountdownSeconds;

    TickCountdown();
    GetWorldTimerManager().SetTimer(CountdownTimer, this, &ACGameModeBase::TickCountdown, 1.f, true);
}

void ACGameModeBase::TickCountdown()
{
    if (bGameEnded)
    {
        GetWorldTimerManager().ClearTimer(CountdownTimer);
        return;
    }

    if (CountdownRemaining <= 0)
    {
        GetWorldTimerManager().ClearTimer(CountdownTimer);
        BeginRules();
        return;
    }

    // High priority so each number replaces the last instead of waiting behind it
    Narrator->SayLine(
        FName(*FString::Printf(TEXT("Countdown_%d"), CountdownRemaining)),
        FText::AsNumber(CountdownRemaining),
        ENarratorPriority::High);

    --CountdownRemaining;
}

void ACGameModeBase::BeginRules()
{
    RuleManager->BeginEnforcement();
    GetWorldTimerManager().SetTimer(HourTimer, this, &ACGameModeBase::EndHour, HourDuration, false);
}

void ACGameModeBase::ReplayRules()
{
    // Ignore the key while he is already talking, so it can't be spammed
    if (bGameEnded || Narrator->IsBusy()) return;

    RuleManager->AnnounceActiveRules();
}

void ACGameModeBase::HandleStageChanged(ESanityStage OldStage, ESanityStage NewStage)
{
    // Lower sanity, lower voice
    switch (NewStage)
    {
    case ESanityStage::High:   Narrator->SetVoicePitch(1.0f);  break;
    case ESanityStage::Medium: Narrator->SetVoicePitch(0.95f); break;
    default:                   Narrator->SetVoicePitch(0.85f); break;
    }

    // Only when sanity gets worse, and only if he isn't busy (Low priority)
    if (static_cast<uint8>(NewStage) > static_cast<uint8>(OldStage) && !bGameEnded)
    {
        if (NewStage == ESanityStage::Medium)
        {
            Narrator->SayLine(TEXT("Stage_Medium"), FText::GetEmpty(), ENarratorPriority::Low);
        }
        else if (NewStage == ESanityStage::Low)
        {
            Narrator->SayLine(TEXT("Stage_Low"), FText::GetEmpty(), ENarratorPriority::Low);
        }
    }
}

void ACGameModeBase::HandleRuleAnnounced(FName RuleID, FText RuleText)
{
    Narrator->SayRule(RuleID, RuleText);
}

void ACGameModeBase::HandleRuleExpired(FName RuleID, FText RuleText)
{
    const FString Text = FString::Printf(TEXT("The rule \"%s\" no longer applies."), *RuleText.ToString());
    Narrator->SayLine(FName(*(FString(TEXT("Expired_")) + RuleID.ToString())), FText::FromString(Text));
}

void ACGameModeBase::HandleRuleBroken(FName RuleID)
{
    if (!bGameEnded)
    {
        Narrator->SayReaction(RuleID);
    } 
}

UCSanityComponent* ACGameModeBase::GetPlayerSanity() const
{
    if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0))
    {
        return Pawn->FindComponentByClass<UCSanityComponent>();
    }
    return nullptr;
}
