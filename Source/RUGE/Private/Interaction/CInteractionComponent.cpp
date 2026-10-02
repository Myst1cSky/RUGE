// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/CInteractionComponent.h"
#include "CInteractable.h"
#include "Characters/Player/CPlayerController.h"

// Sets default values for this component's properties
UCInteractionComponent::UCInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, 
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;
	
	const ACPlayerController* PC = Cast<ACPlayerController>(OwnerPawn->GetController());
	if (!PC) return;
	
	FVector Start;
	FRotator Rotation;
	PC->GetPlayerViewPoint(Start, Rotation);
	const FVector End = Start + Rotation.Vector() * InteractDistance;
	
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(InteractTrace), false, OwnerPawn);
	
	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
	AActor* HitActor = bHit ? Hit.GetActor() : nullptr;
	
	FocusedActor = (HitActor && HitActor->Implements<UCInteractable>()) ? HitActor : nullptr;
	
	if (GEngine && FocusedActor)
	{
		GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, GetCurrentPrompt().ToString());
	}
}

void UCInteractionComponent::TryInteract()
{
	if (ICInteractable* Interactable = Cast<ICInteractable>(FocusedActor.Get()))
	{
		Interactable->Interact(GetOwner());
	}
}

FText UCInteractionComponent::GetCurrentPrompt() const
{
	if (const ICInteractable* Interactable = Cast<ICInteractable>(FocusedActor.Get()))
	{
		return Interactable->GetPromptText();
	}
	return FText::GetEmpty();	
}

