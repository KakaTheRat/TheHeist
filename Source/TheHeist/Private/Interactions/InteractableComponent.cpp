// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactions/InteractableComponent.h"


// Sets default values for this component's properties
UInteractableComponent::UInteractableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UInteractableComponent::RunInteraction(USceneComponent* HitComponent,UInteractionData* Data)
{
	if (HitComponent && Data)
	{
		CurrentlyChosenComponent = HitComponent;
	}
}


void UInteractableComponent::FindAttachedComponent()
{
	if (Owner)
	{
		TArray<USceneComponent*> Components;
		Owner->GetComponents<USceneComponent>(Components);

		for (USceneComponent* Component : Components)
		{
			if (NamesOfComponents.Contains(Component->GetName()))
			{
				AttachedComponents.Add(Component);
			}
		}
	}
}

// Called when the game starts
void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();
	
	//FindAttachedComponent();
	
}


// Called every frame
void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}



