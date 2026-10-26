#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CEndScreenWidget.generated.h"

class UTextBlock;
class UProgressBar;
class UButton;

UCLASS()
class UCEndScreenWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // Call after CreateWidget and before AddToViewport
    void Setup(const FText& Title, const FText& Info, float SanityPercent, float SanityValue);

protected:
    virtual void NativeConstruct() override;

    // These must match the widget names in the editor EXACTLY
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TitleText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> InfoText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> RestartButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> QuitButton;

private:
    UFUNCTION()
    void OnRestartClicked();

    UFUNCTION()
    void OnQuitClicked();
};