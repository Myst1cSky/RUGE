// Fill out your copyright notice in the Description page of Project Settings.


#include "Sanity/CSanityComponent.h"
#include "Engine/Engine.h"

// Sets default values for this component's properties
UCSanityComponent::UCSanityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UCSanityComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentSanity = MaxSanity;
	CurrentStage = CalculateStage(CurrentSanity);
	bDepleted = false;
	
	OnSanityChanged.Broadcast(CurrentSanity, CurrentSanity, 0.f);
	
}

void UCSanityComponent::Drain(float Amount, FName Reason)
{
	if (Amount <= 0.f || bDepleted) return;
	SetSanity(CurrentSanity - Amount, Reason);
}

void UCSanityComponent::Restore(float Amount)
{
	if (Amount <= 0.f || bDepleted) return;
	SetSanity(CurrentSanity + Amount, TEXT("Restore"));
}

void UCSanityComponent::SetSanity(float NewValue, FName Reason)
{
	const float OldValue = CurrentSanity;
	CurrentSanity = FMath::Clamp(NewValue, 0.f, MaxSanity);
	
	if (FMath::IsNearlyEqual(OldValue, CurrentSanity)) return;
	
	if (bShowDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, 
			FString::Printf(TEXT("Sanity %.0f -> %.0f (%s)"), 
				OldValue, CurrentSanity, *Reason.ToString()));
	}
	
	OnSanityChanged.Broadcast(OldValue, CurrentSanity, CurrentSanity - OldValue);
	
	const ESanityStage OldStage = CurrentStage;
	CurrentStage = CalculateStage(CurrentSanity);
	if (OldStage != CurrentStage)
	{
		OnStageChanged.Broadcast(OldStage, CurrentStage);
	}
	
	// Fires only once
	if (CurrentSanity <= 0.f && !bDepleted)
	{
		bDepleted = true;
		OnSanityDepleted.Broadcast();
	}
}

ESanityStage UCSanityComponent::CalculateStage(float Value) const
{
	if (Value <= 0.f) return ESanityStage::Zero;
	if (Value >= HighThreshold) return ESanityStage::High;
	if (Value >= MediumThreshold) return ESanityStage::Medium;
	return ESanityStage::Low;
}

