// Fill out your copyright notice in the Description page of Project Settings.


#include "TimelinesComponent.h"

UTimelinesComponent::UTimelinesComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UTimelinesComponent::PlayTimeline(UCurveFloat* InCurve, float InDuration, bool bReverse)
{
    if (InCurve)
    {

        FOnTimelineFloat ProgressFunction;
        ProgressFunction.BindUFunction(this, FName("HandleTimelineProgress"));
        
        Timeline = FTimeline();
        Timeline.AddInterpFloat(InCurve, ProgressFunction);
        Timeline.SetTimelineLength(InDuration);
        Timeline.SetLooping(false);

        if (bReverse)
        {
            Timeline.ReverseFromEnd();
        }
        else
        {
            Timeline.PlayFromStart();
        }
        
    }
}

void UTimelinesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (Timeline.IsPlaying())
    {
        Timeline.TickTimeline(DeltaTime);
    }
}

void UTimelinesComponent::HandleTimelineProgress(float Value)
{
    OnTimelineProgress.Broadcast(Value);
}