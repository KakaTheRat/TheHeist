// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactions/InteractableComponent.h"
#include "CollectableComponent.generated.h"

/**
 * 
 */
UCLASS()
class THEHEIST_API UCollectableComponent : public UInteractableComponent
{
	GENERATED_BODY()

protected:
	
	//FUNCTIONS
	
	//Interaction logic
	virtual void RunInteraction(USceneComponent* HitComponent, UInteractionData* Data) override;

	//VARIABLES
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category="Interactions")
	UCollectableData* CurrentInteractionData;

	
	
};
