// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CSubtitleWidget.generated.h"

class UTextBlock;

UCLASS()
class UCSubtitleWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void ShowSubtitle(const FText& Text);
	void ClearSubtitle();

protected:
	// Must match the widget name in the editor EXACTLY
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SubtitleText;
};
