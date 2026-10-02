// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CInteractable.generated.h"

/**
 * 
 */
UINTERFACE(MinimalAPI)
class UCInteractable : public UInterface
{
	GENERATED_BODY()
};

class RUGE_API ICInteractable
{
	GENERATED_BODY()
	
public:
	virtual void Interact(AActor* Interactor) = 0;
	virtual FText GetPromptText() const = 0;
};
