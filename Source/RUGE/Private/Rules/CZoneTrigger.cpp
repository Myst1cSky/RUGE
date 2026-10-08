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
	Box->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	Box->SetGenerateOverlapEvents(true);
}

void ACZoneTrigger::BeginPlay()
{
	Super::BeginPlay();
	Box->OnComponentBeginOverlap.AddDynamic(this, &ACZoneTrigger::OnBeginOverlap);
	Box->OnComponentEndOverlap.AddDynamic(this, &ACZoneTrigger::OnEndOverlap);
	
	// If the player already starts inside the zone, record it
	TArray<AActor*> Overlapping;
	Box->GetOverlappingActors(Overlapping, APawn::StaticClass());
	for (AActor* Actor : Overlapping)
	{
		Report(Actor, true);
	}
}

void ACZoneTrigger::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Report(OtherActor, true);
}

void ACZoneTrigger::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Report(OtherActor, false);
}

void ACZoneTrigger::Report(AActor* Other, bool bEntering)
{
	const APawn* Pawn = Cast<APawn>(Other);
	if (!Pawn || !Pawn->IsPlayerControlled()) return;

	if (ACGameModeBase* GameModeBase = GetWorld()->GetAuthGameMode<ACGameModeBase>())
	{
		if (UCRuleManager* RuleManager = GameModeBase->GetRuleManager())
		{
			RuleManager->SetZoneOccupied(ZoneTag, bEntering);

			const FString Suffix = bEntering ? TEXT(".Enter") : TEXT(".Exit");
			RuleManager->ReportEvent(FName(*(ZoneTag.ToString() + Suffix)));
		}
	}
}


