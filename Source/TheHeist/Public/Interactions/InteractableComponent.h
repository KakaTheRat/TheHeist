// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionInterface.h"
#include "Components/ActorComponent.h"
#include "../Structures/Interactions/InteractionData.h"
#include "Widget/Interaction/InteractionWidgetActor.h"
#include "InteractableComponent.generated.h"





USTRUCT(BlueprintType)
struct FInteractionEntry
{
	GENERATED_BODY()

	// Associated scene component name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	FName ComponentName;

	// Array of available interactions types for this component
	UPROPERTY(EditAnywhere,Instanced, BlueprintReadWrite, Category="Interaction")
	TArray<UInteractionData*> Interactions; 
};



USTRUCT(BlueprintType)
struct FComponentInteractions
{
	GENERATED_BODY()

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	FName SlotName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	TArray<FInteractionEntry> ComponentEntries;
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEHEIST_API UInteractableComponent : public UActorComponent, public IInteractionInterface
{
	GENERATED_BODY()

public:

	// List of names for every component attached (depending on the NumComponent)
	UPROPERTY(EditAnywhere, Category="Interactable")
	TArray<FString> NamesOfComponents;

protected:

	//VARIABLES
	
	//Scene components which are accessing to this component
	UPROPERTY(Blueprintable,BlueprintReadWrite,  Category=Interactable)
	TArray<USceneComponent*>  AttachedComponents;

	//Actor owning the component
	UPROPERTY()
	AActor* Owner;

	//Component targeted by the player
	UPROPERTY(BlueprintReadWrite, Category="Interactable")
	USceneComponent* CurrentlyChosenComponent;

	//Current entry treated
	FInteractionEntry* CurrentEntry = nullptr;

	//Array of interactions config created so far
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	TArray<FInteractionEntry> InteractionsConfig;

	//Array of interactions config created so far with the perception activated
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	TArray<FInteractionEntry> InteractionsConfigPerceptionAI;


	//WIDGET
	
	// Widget Class to instantiate. Widget for the interaction system
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<AInteractionWidgetActor> InteractionWidgetClass;
	
	AInteractionWidgetActor* WidgetInstance = nullptr;
	
	

	//FUNCTIONS
	
	// Called when the game starts
	virtual void BeginPlay() override;
	
	//Function to execute interaction (taking as an input the interaction text, received from the widget
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void InteractWithObject(const FString m_InteractText);

	//Implementation for the interact interface function
	virtual void Interact_Implementation(USceneComponent* HitComponent) override;

	//Implementation for the interact AI interface function
	virtual void InteractAI_Implementation() override;
	

public:
	
	// Sets default values for this component's properties
	UInteractableComponent();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	//Array of interactions
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category="Interactions")
	TArray<UInteractionData*> Interactions;

};
