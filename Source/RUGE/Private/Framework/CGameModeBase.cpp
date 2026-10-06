// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameModeBase.h"
#include "Rules/CRuleManager.h"
#include "Sanity/CSanityComponent.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

ACGameModeBase::ACGameModeBase()
{
	RuleManager = CreateDefaultSubobject<UCRuleManager>(TEXT("RuleManager"));
}

void ACGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// Short pause so the player can get oriented before Round 1
	GetWorldTimerManager().SetTimer(RoundTimer,
		FTimerDelegate::CreateUObject(this, &ACGameModeBase::StartRound, 1), 3.f, false);
}

void ACGameModeBase::StartRound(int32 Round)
{
	CurrentRound = Round;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
			FString::Printf(TEXT("=== ROUND %d ==="), Round));
	}

	RuleManager->StartRound(RuleTable, Round);
	GetWorldTimerManager().SetTimer(RoundTimer, this, &ACGameModeBase::EndRound, RoundDuration, false);
}

void ACGameModeBase::EndRound()
{
	const bool bFlawless = RuleManager->EndRound();

	if (UCSanityComponent* Sanity = GetPlayerSanity())
	{
		Sanity->Restore(SanityRecoveryPerRound);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
			FString::Printf(TEXT("Round %d over (%s)"), CurrentRound, bFlawless ? TEXT("flawless") : TEXT("mistakes made")));
	}

	if (CurrentRound < MaxRounds)
	{
		GetWorldTimerManager().SetTimer(RoundTimer,
			FTimerDelegate::CreateUObject(this, &ACGameModeBase::StartRound, CurrentRound + 1), BreakDuration, false);
	}
	else if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("Night complete"));
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
