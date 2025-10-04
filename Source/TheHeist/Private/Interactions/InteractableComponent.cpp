// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactions/InteractableComponent.h"

#include "LandscapeGizmoActiveActor.h"


// Sets default values for this component's properties
UInteractableComponent::UInteractableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

//Interaction interface interact function
void UInteractableComponent::Interact_Implementation(USceneComponent* HitComponent)
{
	IInteractionInterface::Interact_Implementation(HitComponent);
	
	
	CurrentlyChosenComponent = HitComponent;

	if (InteractionWidgetClass && GetWorld())
	{
		//Destroys Widget attached
		if (WidgetInstance)
		{
			WidgetInstance->Destroy();
			WidgetInstance = nullptr;
		}

		FActorSpawnParameters SpawnParams;

		//Creates a new widget 
		WidgetInstance = GetWorld()->SpawnActor<AInteractionWidgetActor>(
		InteractionWidgetClass,
		FVector(Owner->GetActorLocation().X,Owner->GetActorLocation().Y,100  + Owner->GetActorLocation().Z),   // position
		FRotator::ZeroRotator,
		SpawnParams
	);
	}

	//Widget setup (creating different interactions)
	for (FInteractionEntry& Entry : InteractionsConfig)
	{
		if (Entry.ComponentName == CurrentlyChosenComponent->GetName())
		{
			CurrentEntry = &Entry;
			
			for (UInteractionData* Data : Entry.Interactions)
			{
				if (Data)
				{
					//Allows to add options for the widget
					WidgetInstance->AddInteractionEntry(Data->InteractText);
				}
			}
		}
	}

	//Subscribe to the onclick event of the widget
	WidgetInstance->MyWidget->OnInteractionClicked.AddDynamic(this, &UInteractableComponent::InteractWithObject);
}

//Execute interaction based on the type
void UInteractableComponent::InteractWithObject(const FString m_InteractText)
{
	for (UInteractionData* Data : CurrentEntry->Interactions)
	{
		if (Data && Data->InteractText == m_InteractText)
		{
				// Execute interaction
				Data->ExecuteInteraction(Owner, CurrentlyChosenComponent);
				break;

		}

	}
	
	//Widget instance destroyed 
	if (WidgetInstance)
	{
		WidgetInstance->Destroy();
		WidgetInstance = nullptr;
	}
	
	
}

// Called when the game starts
void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();
}


// Called every frame
void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Allows Interactions Datas to tick (empty function overrided, means empty by default)
	for (UInteractionData* Data : Interactions)
	{
		if (Data)
		{
			Data->Tick(DeltaTime);
		}
	}
}



