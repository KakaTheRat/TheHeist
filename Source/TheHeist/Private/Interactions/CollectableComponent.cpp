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

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	PlayerRef = PC->GetPawn();

	if (PlayerRef)
	{
		UPlayerInventory* Inventory = PlayerRef->FindComponentByClass<UPlayerInventory>();
		if (Inventory)
		{
			Inventory->AddItem(GadgetClass);
			GetOwner()->Destroy();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No InventoryComponent found on %s"), *PlayerRef->GetName());
		}
	}
}

