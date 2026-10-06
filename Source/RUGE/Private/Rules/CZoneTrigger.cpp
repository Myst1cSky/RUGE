// Fill out your copyright notice in the Description page of Project Settings.


#include "Rules/CZoneTrigger.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "Framework/CGameModeBase.h"
#include "Rules/CRuleManager.h"

// Sets default values
ACZoneTrigger::ACZoneTrigger()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(Box);
	Box->SetBoxExtent(FVector(200.f, 200.f, 100.f));
	Box->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	Box->SetGenerateOverlapEvents(true);
}

void ACZoneTrigger::BeginPlay()
{
	Super::BeginPlay();
	Box->OnComponentBeginOverlap.AddDynamic(this, &ACZoneTrigger::OnBeginOverlap);
	Box->OnComponentEndOverlap.AddDynamic(this, &ACZoneTrigger::OnEndOverlap);
}

void ACZoneTrigger::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Report(OtherActor, TEXT(".Enter"));
}

void ACZoneTrigger::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Report(OtherActor, TEXT(".Exit"));
}

void ACZoneTrigger::Report(AActor* Other, const TCHAR* Suffix)
{
	const APawn* Pawn = Cast<APawn>(Other);
	if (!Pawn || !Pawn->IsPlayerControlled()) return;

	if (ACGameModeBase* GameModeBase = GetWorld()->GetAuthGameMode<ACGameModeBase>())
	{
		if (UCRuleManager* RuleManager = GameModeBase->GetRuleManager())
		{
			RuleManager->ReportEvent(FName(*FString::Printf(TEXT("%s%s"), *ZoneTag.ToString(), Suffix)));
		}
	}
}


