// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/CPlayerCharacter.h"
#include "Characters/Player/CPlayerController.h"
#include "Components/SpotlightComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interaction/CInteractionComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACPlayerCharacter::ACPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(720.f);
	
	CreateFlashlight();
	
	InteractionComponent = CreateDefaultSubobject<UCInteractionComponent>(TEXT("InteractionComponent"));
}

// Called when the game starts or when spawned
void ACPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateFlashlightRotaion();

}

void ACPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = 
		GetController<ACPlayerController>()->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	
	if (EnhancedInputLocalPlayerSubsystem)
	{
		EnhancedInputLocalPlayerSubsystem->ClearAllMappings();
		EnhancedInputLocalPlayerSubsystem->AddMappingContext(GameplayMappingContext, 0);
	}
}

void ACPlayerCharacter::CreateFlashlight()
{
	Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
	Flashlight->SetupAttachment(RootComponent);
	Flashlight->SetRelativeLocation(FVector(30.f, 0.f, 60.f));
	Flashlight->Intensity = 5000.f;
	Flashlight->InnerConeAngle = 10.f;
	Flashlight->OuterConeAngle = 25.f;
	Flashlight->AttenuationRadius = 2000.f;
	Flashlight->SetVisibility(false);
}


// Called to bind functionality to input
void ACPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::HandleLookInput);
		EnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::HandleMoveInput);
		EnhancedInputComponent->BindAction(InteractInputAction, ETriggerEvent::Started, this, &ACPlayerCharacter::HandleInteractInput);
		
		if (FlashlightInputAction)
		{
			EnhancedInputComponent->BindAction(FlashlightInputAction, ETriggerEvent::Started, this, &ACPlayerCharacter::ToggleFlashlight);
		}
	}
	

}

void ACPlayerCharacter::HandleLookInput(const struct FInputActionValue& InputActionValue)
{
	FVector2D InputAction = InputActionValue.Get<FVector2D>();
	AddControllerYawInput(InputAction.X);
	AddControllerPitchInput(InputAction.Y);
}

void ACPlayerCharacter::HandleMoveInput(const struct FInputActionValue& InputActionValue)
{
	FVector2D InputAction = InputActionValue.Get<FVector2D>();
	InputAction.Normalize();
	
	AddMovementInput(GetMoveFwdDir() * InputAction.Y + GetRightDir() * InputAction.X);
}

void ACPlayerCharacter::HandleInteractInput(const struct FInputActionValue& InputActionValue)
{
	if (InteractionComponent)
	{
		InteractionComponent->TryInteract();
	}
}

FVector ACPlayerCharacter::GetRightDir() const
{
	return GetActorRightVector();
}

FVector ACPlayerCharacter::GetLookFwdDir() const
{
	return GetActorForwardVector();
}

FVector ACPlayerCharacter::GetMoveFwdDir() const
{
	return FVector::CrossProduct(GetRightDir(), FVector::UpVector);
}

void ACPlayerCharacter::ToggleFlashlight()
{
	bIsFlashlightOn = !bIsFlashlightOn;
	Flashlight->SetVisibility(bIsFlashlightOn);
}

void ACPlayerCharacter::UpdateFlashlightRotaion()
{
	if (bIsFlashlightOn)
	{
		Flashlight->SetWorldRotation(GetControlRotation());
	}
}

