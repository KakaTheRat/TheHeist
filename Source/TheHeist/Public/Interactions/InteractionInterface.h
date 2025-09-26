// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class THEHEIST_API IInteractionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	//Interact Function, called whenever an object is being interacted with.
	//Takes the hit component as an input
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interactable")
	void Interact(USceneComponent* HitComponent);
};
