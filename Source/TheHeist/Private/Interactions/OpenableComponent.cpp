// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactions/OpenableComponent.h"

void UOpenableComponent::Interact_Implementation(USceneComponent* HitComponent)
{
	Open();
}

void UOpenableComponent::Open()
{

	if (bIsOpened)
	{
		InteractText = "Close";
	}
	else
	{
		InteractText = "Open";
	}
	
}
