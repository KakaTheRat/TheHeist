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


void UInteractableComponent::Interact_Implementation(USceneComponent* HitComponent)
{
	//
}

void UInteractableComponent::FindAttachedComponent()
{
	if (AActor* Owner = GetOwner())
	{
		TArray<USceneComponent*> Components;
		Owner->GetComponents<USceneComponent>(Components);

		for (USceneComponent* Component : Components)
		{
			for (const FString& NameOfComponent : NamesOfComponents)
			{
				if (Component->GetName() == NameOfComponent)
				{
					AttachedComponent = Component;
					return;
				}
			}
		}
	}
}

// Called when the game starts
void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	FindAttachedComponent();
	
}


// Called every frame
void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

#if WITH_EDITOR
void UInteractableComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != nullptr) 
		? PropertyChangedEvent.Property->GetFName() 
		: NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UInteractableComponent, AmountComponents))
	{
		int32 CurrentSize = NamesOfComponents.Num();

		if (AmountComponents > CurrentSize)
		{
			// Add blank entries
			for (int32 i = CurrentSize; i < AmountComponents; ++i)
			{
				NamesOfComponents.Add(TEXT(""));
			}
		}
		else if (AmountComponents < CurrentSize)
		{
			// Cut the list
			NamesOfComponents.SetNum(AmountComponents);
		}
	}
}
#endif

