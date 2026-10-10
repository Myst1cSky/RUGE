// Fill out your copyright notice in the Description page of Project Settings.


#include "Narrator/CSubtitleWidget.h"
#include "Components/TextBlock.h"


void UCSubtitleWidget::ShowSubtitle(const FText& Text)
{
	if (!SubtitleText) return;
	SubtitleText->SetText(Text);
	SubtitleText->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UCSubtitleWidget::ClearSubtitle()
{
	if (!SubtitleText) return;
	SubtitleText->SetVisibility(ESlateVisibility::Collapsed);
}
