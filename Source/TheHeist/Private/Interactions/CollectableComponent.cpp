// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactions/CollectableComponent.h"


void UCollectableComponent::RunInteraction(USceneComponent* HitComponent, UInteractionData* Data)
{
	Super::RunInteraction(HitComponent, Data);
	
	UCollectableData* CollectableData = Cast<UCollectableData>(Data);
	if (!CollectableData)
	{
		UE_LOG(LogTemp, Warning, TEXT("RunInteraction: Wrong Data type!"));
		return;
	}
}

