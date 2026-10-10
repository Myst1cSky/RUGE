// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameModeBase.h"
#include "Rules/CRuleManager.h"
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
    }

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

    RuleManager->StartHour(Hour);
    GetWorldTimerManager().SetTimer(HourTimer, this, &ACGameModeBase::EndHour, HourDuration, false);
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
    RuleManager->StopRules();

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red,
            FString::Printf(TEXT("GAME OVER - You lost your mind at Hour %d"), CurrentHour));
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
