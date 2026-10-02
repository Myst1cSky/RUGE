// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CPlayerCharacter.generated.h"


class USpotLightComponent;
class USoundBase;

UCLASS()
class ACPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACPlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PawnClientRestart() override;
	void CreateFlashlight();
	
	//-----------------------------------------------------//
	//                     Input                          //
	//----------------------------------------------------//
	
private:
	void HandleLookInput(const struct FInputActionValue& InputActionValue);
	void HandleMoveInput(const struct FInputActionValue& InputActionValue);
	
	FVector GetRightDir() const;
	FVector GetLookFwdDir() const;
	FVector GetMoveFwdDir() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* GameplayMappingContext;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* JumpInputAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* LookInputAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* MoveInputAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* FlashlightInputAction;
	
	//-----------------------------------------------------//
	//                     Flashlight                     //
	//----------------------------------------------------//
	
protected:
	UPROPERTY(VisibleAnywhere, Category = "Flashlight")
	USpotLightComponent* Flashlight;
	
	UPROPERTY(EditAnywhere, Category = "Flashlight")
	USoundBase* FlashlightSound;
	
	bool bIsFlashlightOn = false;
	
	void ToggleFlashlight();
	void UpdateFlashlightRotaion();
};
