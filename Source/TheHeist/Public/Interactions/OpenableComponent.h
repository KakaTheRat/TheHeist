// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactions/InteractableComponent.h"
#include "Components/TimelineComponent.h"
#include "OpenableComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEHEIST_API UOpenableComponent : public UInteractableComponent
{
	GENERATED_BODY()

protected:
	
	//VARIABLES

	FTimeline Timeline;
	
	// Function called by the timeline
	UFUNCTION()
	void HandleProgress(float Value);

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category="Interactions")
	UOpenableData* CurrentInteractionData;
	
	
	//FUNCTIONS

	virtual void BeginPlay() override;

	// Launch opening
	UFUNCTION(BlueprintCallable, Category="TimelineTest")
	void PlayForward();

	// Launch closing
	UFUNCTION(BlueprintCallable, Category="TimelineTest")
	void PlayReverse();

	//Interaction logic
	virtual void RunInteraction(USceneComponent* HitComponent, UInteractionData* Data) override;
	
	//Execute logic to open or close
	void Open();

	

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
};
