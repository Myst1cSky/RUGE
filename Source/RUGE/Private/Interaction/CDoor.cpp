// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/CDoor.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ACDoor::ACDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	SetRootComponent(Pivot);
	
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(Pivot);
}

// Called when the game starts or when spawned
void ACDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator Current = Pivot->GetRelativeRotation();
	if (!FMath::IsNearlyEqual(Current.Yaw, TargetYaw, 0.1f))
	{
		Current.Yaw = FMath::FInterpConstantTo(Current.Yaw, TargetYaw, DeltaTime, RotationSpeed);
		Pivot->SetRelativeRotation(Current);
	}
}

void ACDoor::Interact(AActor* Interactor)
{
	if (bIsOpen)
	{
		bIsOpen = false;
		TargetYaw = 0.f;
		return;
		//Closing: always return to 0
	}
	
	bIsOpen = true;
	
	const FVector HingeToDoor = DoorMesh->Bounds.Origin - GetActorLocation();
	const FVector HingeToPlayer = Interactor->GetActorLocation() - GetActorLocation();
	
	const float Cross = HingeToDoor.X * HingeToPlayer.Y - HingeToDoor.Y * HingeToPlayer.X;
	
	TargetYaw = (Cross >= 0.f) ? -OpenAngle : OpenAngle;
	// Swing away from the player
}

FText ACDoor::GetPromptText() const
{
	return bIsOpen ? FText::FromString(TEXT("Press E to close"))
	: FText::FromString(TEXT("Press E to open"));
}

