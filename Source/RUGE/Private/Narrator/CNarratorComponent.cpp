// Fill out your copyright notice in the Description page of Project Settings.


#include "Narrator/CNarratorComponent.h"
#include "Narrator/CSubtitleWidget.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/AudioComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Sound/SoundBase.h"

UCNarratorComponent::UCNarratorComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCNarratorComponent::Init(UDataTable* InTable, TSubclassOf<UCSubtitleWidget> WidgetClass)
{
    LineTable = InTable;

    if (WidgetClass && !SubtitleWidget)
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            SubtitleWidget = CreateWidget<UCSubtitleWidget>(PC, WidgetClass);
            if (SubtitleWidget)
            {
                SubtitleWidget->AddToViewport(10);
                SubtitleWidget->ClearSubtitle();
            }
        }
    }
}

bool UCNarratorComponent::FindLine(FName LineID, FCQueuedLine& OutLine) const
{
    if (!LineTable) return false;

    const FCNarratorLine* Row = LineTable->FindRow<FCNarratorLine>(LineID, TEXT("Narrator"), false);
    if (!Row) return false;

    OutLine.Subtitle = Row->Subtitle;
    OutLine.Sound = Row->Sound;
    OutLine.PauseAfter = Row->PauseAfter;
    return true;
}

void UCNarratorComponent::SayLine(FName LineID, const FText& Fallback, ENarratorPriority Priority)
{
    FCQueuedLine Line;
    if (!FindLine(LineID, Line))
    {
        if (Fallback.IsEmpty()) return;
        Line.Subtitle = Fallback;
        Line.PauseAfter = 1.f;
    }
    Enqueue(Line, Priority);
}

void UCNarratorComponent::SayHour(int32 Hour, const FText& Fallback)
{
    SayLine(FName(*FString::Printf(TEXT("Hour_%d"), Hour)), Fallback);
}

void UCNarratorComponent::SayRule(FName RuleID, const FText& RuleText)
{
    SayLine(FName(*(FString(TEXT("Rule_")) + RuleID.ToString())), RuleText);
}

void UCNarratorComponent::SayReaction(FName RuleID)
{
    const UWorld* World = GetWorld();
    if (!World) return;

    const float Now = World->GetTimeSeconds();
    if (Now - LastReactionTime < ReactionCooldown) return;
    LastReactionTime = Now;

    FCQueuedLine Line;

    // A custom reaction for this rule, if one exists
    if (!FindLine(FName(*(FString(TEXT("Broke_")) + RuleID.ToString())), Line))
    {
        // Otherwise a random generic one, never the same twice in a row
        int32 Pick = 1;
        if (GenericReactionCount > 1)
        {
            do { Pick = FMath::RandRange(1, GenericReactionCount); }
            while (Pick == LastGenericIndex);
        }
        LastGenericIndex = Pick;

        if (!FindLine(FName(*FString::Printf(TEXT("Reaction_Generic_%d"), Pick)), Line))
        {
            Line.Subtitle = FText::FromString(TEXT("That was a mistake."));
            Line.PauseAfter = 1.f;
        }
    }

    Enqueue(Line, ENarratorPriority::Normal, /*bToFront=*/true);
}

void UCNarratorComponent::Enqueue(const FCQueuedLine& Line, ENarratorPriority Priority, bool bToFront)
{
    if (Priority == ENarratorPriority::Low && bBusy) return;
    if (Priority == ENarratorPriority::High) StopAll();

    if (bToFront) Queue.Insert(Line, 0);
    else          Queue.Add(Line);

    if (!bBusy) PlayNext();
}

void UCNarratorComponent::PlayNext()
{
    if (Queue.IsEmpty())
    {
        const bool bWasBusy = bBusy;
        bBusy = false;
        if (bWasBusy)
        {
            OnQueueEmpty.Broadcast();
        }
        return;
    }

    bBusy = true;
    const FCQueuedLine Line = Queue[0];
    Queue.RemoveAt(0);

    if (SubtitleWidget) SubtitleWidget->ShowSubtitle(Line.Subtitle);

    // Default reading time from text length
    float Duration = FMath::Max(MinTextDuration, Line.Subtitle.ToString().Len() * SecondsPerCharacter);

    if (USoundBase* Sound = Line.Sound.LoadSynchronous())
    {
        ActiveSound = UGameplayStatics::SpawnSound2D(this, Sound, 1.f, VoicePitch);

        const float SoundLength = Sound->GetDuration();
        if (SoundLength > 0.f && SoundLength < 60.f)
        {
            Duration = SoundLength;
        }
    }

    GetWorld()->GetTimerManager().SetTimer(NarratorTimer,
        FTimerDelegate::CreateUObject(this, &UCNarratorComponent::FinishLine, Line.PauseAfter),
        Duration, false);
}

void UCNarratorComponent::FinishLine(float PauseAfter)
{
    if (SubtitleWidget) SubtitleWidget->ClearSubtitle();

    if (PauseAfter > 0.f)
    {
        GetWorld()->GetTimerManager().SetTimer(NarratorTimer, this, &UCNarratorComponent::PlayNext, PauseAfter, false);
    }
    else
    {
        PlayNext();
    }
}

void UCNarratorComponent::StopAll()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(NarratorTimer);
    }

    Queue.Reset();
    bBusy = false;

    if (ActiveSound)
    {
        ActiveSound->Stop();
        ActiveSound = nullptr;
    }
    if (SubtitleWidget) SubtitleWidget->ClearSubtitle();
}