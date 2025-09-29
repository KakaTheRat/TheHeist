// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactions/InteractableComponent.h"
#include "Components/LightComponent.h"
#include "Components/AudioComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h" 
#include "ActivableComponent.generated.h"


/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEHEIST_API UActivableComponent : public UInteractableComponent
{
	GENERATED_BODY()

	
	
protected: 

	void Activate();

	//Logic for a basic light activation
	void ActivateLight();

	//Logic for a basic sound activation
	void ActivateSound();

	void ReportNoiseEvent();

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category="Interactions")
	UActivableData* CurrentInteractionData;

	//Interaction logic
	virtual void RunInteraction(USceneComponent* HitComponent, UInteractionData* Data) override;

	
	
public:
	
	
};
