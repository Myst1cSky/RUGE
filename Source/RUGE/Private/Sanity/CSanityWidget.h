// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Sanity/CSanityComponent.h"
#include "CSanityWidget.generated.h"

/**
 * 
 */

class UProgressBar;
class UTextBlock;

UCLASS()
class UCSanityWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitWidget(UCSanityComponent* InSanityComponent);
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> SanityBar;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SanityText;
	
	UPROPERTY(EditDefaultsOnly, Category = "Sanity")
	float BarInterpSpeed = 5.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Sanity | Colors")
	FLinearColor HighColor = FLinearColor(0.2f, 0.8f, 0.3f);
	
	UPROPERTY(EditDefaultsOnly, Category = "Sanity | Colors")
	FLinearColor MediumColor = FLinearColor(0.9f, 0.7f, 0.1f);
	
	UPROPERTY(EditDefaultsOnly, Category = "Sanity | Colors")
	FLinearColor LowColor = FLinearColor(0.85f, 0.15f, 0.1f);
	
private:
	UFUNCTION()
	void HandleSanityChanged(float OldSanity, float NewSanity, float Delta);
	
	UFUNCTION()
	void HandleStageChanged(ESanityStage OldStage, ESanityStage NewStage);
	
	void RefreshText(float Value);
	void ApplyStageColor(ESanityStage Stage);
	
	UPROPERTY()
	TObjectPtr<UCSanityComponent> SanityComponent;
	
	float TargetPercent = 1.f;
	float DisplayedPercent = 1.f;
};
