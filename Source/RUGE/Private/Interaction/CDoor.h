// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CInteractable.h"
#include "GameFramework/Actor.h"
#include "CDoor.generated.h"

UCLASS()
class ACDoor : public AActor , public ICInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACDoor();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interact(AActor* Interactor) override;
	virtual FText GetPromptText() const override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Pivot;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr <UStaticMeshComponent> DoorMesh;
	
	UPROPERTY(EditAnywhere, Category = "Door")
	float OpenAngle = 90.f;
	
	UPROPERTY(EditAnywhere, Category = "Door")
	float RotationSpeed = 180.f; // degrees per second, lower = slower swing
private:
	bool bIsOpen = false;
	float TargetYaw = 0.f;
};
