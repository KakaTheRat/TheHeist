// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactions/InteractableComponent.h"
#include "OpenableComponent.generated.h"

/**
 * 
 */
UCLASS()
class THEHEIST_API UOpenableComponent : public UInteractableComponent
{
	GENERATED_BODY()

public:
	
	//Interact override function
	virtual void Interact_Implementation(USceneComponent* HitComponent) override;

protected:

	//***Variables***//

	bool bIsOpened;
	
	//***Fonctions***//

	//Execute logic to open or close
	void Open();
	
};
