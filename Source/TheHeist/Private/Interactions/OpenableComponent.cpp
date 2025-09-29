// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactions/OpenableComponent.h"

void UOpenableComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UOpenableComponent::HandleProgress(float Value)
{
	if (CurrentlyChosenComponent)
	{
		FRotator NewRotation = FRotator(0.f, Value * 90.f, 0.f);
		CurrentlyChosenComponent->SetRelativeRotation(NewRotation);
	}
}

void UOpenableComponent::RunInteraction(USceneComponent* HitComponent, UInteractionData* Data)
{
	Super::RunInteraction(HitComponent, Data);
	
	UOpenableData* OpenableData = Cast<UOpenableData>(Data);
	if (!OpenableData)
	{
		UE_LOG(LogTemp, Warning, TEXT("RunInteraction: Wrong Data type!"));
		return;
	}

	// On peut stocker le pointeur typé pour toute la durée de l'interaction
	CurrentInteractionData = OpenableData;

	if (CurrentInteractionData->Curve)
	{
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("HandleProgress"));
        
		Timeline = FTimeline(); // reset propre si relancé
		Timeline.AddInterpFloat(CurrentInteractionData->Curve, ProgressFunction);
		Timeline.SetTimelineLength(CurrentInteractionData->Duration);
		Timeline.SetLooping(false);
	}
	
	Open();
}

void UOpenableComponent::Open()
{
	
	if (CurrentInteractionData->bIsOpened)
	{
		CurrentInteractionData->InteractText = "Close";
		PlayReverse();
	}
	else
	{
		CurrentInteractionData->InteractText = "Open";
		PlayForward();
	}
	CurrentInteractionData->bIsOpened = !CurrentInteractionData->bIsOpened;
}

void UOpenableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Timeline.TickTimeline(DeltaTime);
}

void UOpenableComponent::PlayForward()
{
	if (CurrentInteractionData->Curve)
	{
		Timeline.PlayFromStart();
	}
}

void UOpenableComponent::PlayReverse()
{
	if (CurrentInteractionData->Curve)
	{
		Timeline.ReverseFromEnd();
	}
}
