// Fill out your copyright notice in the Description page of Project Settings.


#include "Sanity/CSanityWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void UCSanityWidget::InitWidget(UCSanityComponent* InSanityComponent)
{
	if (!InSanityComponent) return;
	
	SanityComponent = InSanityComponent;
	SanityComponent->OnSanityChanged.AddDynamic(this, &UCSanityWidget::HandleSanityChanged);
	SanityComponent->OnStageChanged.AddDynamic(this, &UCSanityWidget::HandleStageChanged);
	
	TargetPercent = DisplayedPercent = SanityComponent->GetSanityPercent();
	if (SanityBar) SanityBar->SetPercent(DisplayedPercent);
	RefreshText(SanityComponent->GetSanity());
	ApplyStageColor(SanityComponent->GetStage());
	
}

void UCSanityWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (!FMath::IsNearlyEqual(DisplayedPercent, TargetPercent, 0.001f))
	{
		DisplayedPercent = FMath::FInterpTo(DisplayedPercent, TargetPercent, InDeltaTime, BarInterpSpeed);
		if (SanityBar) SanityBar->SetPercent(DisplayedPercent);
	}
}

void UCSanityWidget::NativeDestruct()
{
	if (SanityComponent)
	{
		SanityComponent->OnSanityChanged.RemoveDynamic(this, &UCSanityWidget::HandleSanityChanged);
		SanityComponent->OnStageChanged.RemoveDynamic(this, &UCSanityWidget::HandleStageChanged);
	}
	
	Super::NativeDestruct();
}

void UCSanityWidget::HandleSanityChanged(float OldSanity, float NewSanity, float Delta)
{
	if (SanityComponent)
	{
		TargetPercent = SanityComponent->GetSanityPercent();
	}
	
	RefreshText(NewSanity);
}

void UCSanityWidget::HandleStageChanged(ESanityStage OldStage, ESanityStage NewStage)
{
	ApplyStageColor(NewStage);
}

void UCSanityWidget::RefreshText(float Value)
{
	if (SanityText)
	{
		SanityText->SetText(FText::FromString(FString::Printf(TEXT("%d"), FMath::CeilToInt(Value))));
	}
}

void UCSanityWidget::ApplyStageColor(ESanityStage Stage)
{
	if (!SanityBar) return;
	
	switch (Stage)
	{
	case ESanityStage::High:   SanityBar->SetFillColorAndOpacity(HighColor);   break;
	case ESanityStage::Medium: SanityBar->SetFillColorAndOpacity(MediumColor); break;
	default:                   SanityBar->SetFillColorAndOpacity(LowColor);    break;
	}
}
