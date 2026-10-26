// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CEndScreenWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"

void UCEndScreenWidget::Setup(const FText& Title, const FText& Info, float SanityPercent, float SanityValue)
{
	if (TitleText) TitleText->SetText(Title);
	if (InfoText) InfoText->SetText(Info);
}

void UCEndScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton) RestartButton->OnClicked.AddDynamic(this, &UCEndScreenWidget::OnRestartClicked);
	if (QuitButton)    QuitButton->OnClicked.AddDynamic(this, &UCEndScreenWidget::OnQuitClicked);
}

void UCEndScreenWidget::OnRestartClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}

	UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this, true)));
}

void UCEndScreenWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}