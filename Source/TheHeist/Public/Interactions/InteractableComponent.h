// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionInterface.h"
#include "Components/ActorComponent.h"
#include "../Structures/Interactions/InteractionData.h"
#include "InteractableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEHEIST_API UInteractableComponent : public UActorComponent, public IInteractionInterface
{
	GENERATED_BODY()

public:

	// List of names for every component attached (depending on the NumComponent)
	UPROPERTY(EditAnywhere, Category="Interactable")
	TArray<FString> NamesOfComponents;

	//Text printed when interacted with
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Interactable")
	FString InteractText;

protected:

	//VARIABLES
	
	//Scene components which are accessing to this component
	UPROPERTY(Blueprintable,BlueprintReadWrite,  Category=Interactable)
	TArray<USceneComponent*>  AttachedComponents;

	//Actor owning the component
	UPROPERTY()
	const AActor* Owner;

	//Component targeted by the player
	UPROPERTY(BlueprintReadWrite, Category="Interactable")
	USceneComponent* CurrentlyChosenComponent;

	
	//FUNCTIONS
	
	// Called when the game starts
	virtual void BeginPlay() override;
	

private :
	
	//Finds the attached component
	void FindAttachedComponent();

public:
	
	// Sets default values for this component's properties
	UInteractableComponent();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	//Logic to run an interaction
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void RunInteraction(USceneComponent* HitComponent, UInteractionData* Data);

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category="Interactions")
	TArray<UInteractionData*> Interactions;
	
	// Cherche une interaction par le nom du component
	UFUNCTION(BlueprintCallable, Category="Interactions")
	UInteractionData* GetInteractionByComponentName(FString HitComponentName)
	{
		for (UInteractionData* Data : Interactions)
		{
			if (Data && Data->AttachedComponentName == HitComponentName)
			{
				return Data;
			}
		}
		return nullptr;
	}
};
