// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CSanityComponent.generated.h"

UENUM(BlueprintType)
enum class ESanityStage : uint8
{
	High,
	Medium,
	Low,
	Zero
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSanityChanged, float, OldSanity, float, NewSanity, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSanityStageChanged, ESanityStage, OldStage, ESanityStage, NewStage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSanityDepleted);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCSanityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCSanityComponent();
	
	UFUNCTION(BlueprintCallable, Category = "Sanity")
	void Drain(float Amount, FName Reason = NAME_None);
	
	UFUNCTION(BlueprintCallable, Category = "Sanity")
	void Restore(float Amount);
	
	float GetSanity() const { return CurrentSanity; }
	float GetMaxSanity() const { return MaxSanity; }
	float GetSanityPercent() const { return MaxSanity > 0.f ? CurrentSanity / MaxSanity : 0.f; }
	ESanityStage GetStage() const { return CurrentStage; }
	
	UPROPERTY(BlueprintAssignable, Category = "Sanity")
	FOnSanityChanged OnSanityChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Sanity")
	FOnSanityStageChanged OnStageChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Sanity")
	FOnSanityDepleted OnSanityDepleted;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = "Sanity")
	float MaxSanity = 100.f;

	UPROPERTY(EditAnywhere, Category = "Sanity | Stages")
	float HighThreshold = 70.f;
	
	UPROPERTY(EditAnywhere, Category = "Sanity | Stages")
	float MediumThreshold = 40.f;
	
	UPROPERTY(EditAnywhere, Category = "Sanity | Debug")
	bool bShowDebugMessages = true;

private:
	void SetSanity(float NewValue, FName Reason);
	ESanityStage CalculateStage(float Value) const;
	
	float CurrentSanity = 100.f;
	ESanityStage CurrentStage = ESanityStage::High;
	bool bDepleted = false;
};
