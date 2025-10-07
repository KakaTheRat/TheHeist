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

void UInteractableComponent::InteractAI_Implementation()
{
	IInteractionInterface::InteractAI_Implementation();
	
	for (FInteractionEntry& Entry : InteractionsConfigPerceptionAI)
	{
		USceneComponent* TargetComponent = nullptr;
		for (USceneComponent* Comp : AttachedComponents)
		{
			if (Comp && Comp->GetName() == Entry.ComponentName.ToString())
			{
				TargetComponent = Comp;
				break;
			}
		}
		if (!TargetComponent) continue;

		for (UInteractionData* Data : Entry.Interactions)
		{
			if (Data) Data->ExecuteInteraction(Owner, TargetComponent);
		}
	}
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

	
	AttachedComponents.Empty();
	TArray<USceneComponent*> Components;
	Owner->GetComponents<USceneComponent>(Components);
	for (USceneComponent* Comp : Components)
	{
		if (Comp)
		{
			AttachedComponents.Add(Comp);
		}
	}

	InteractionsConfigPerceptionAI.Empty();
	for (FInteractionEntry& Entry : InteractionsConfig)
	{
		FInteractionEntry AIEntry;
		AIEntry.ComponentName = Entry.ComponentName;
		for (UInteractionData* Data : Entry.Interactions)
		{
			if (Data && Data->bCanAlertGuards)
			{
				AIEntry.Interactions.Add(Data);
			}
		}
		if (AIEntry.Interactions.Num() > 0)
		{
			InteractionsConfigPerceptionAI.Add(AIEntry);
		}
	}
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



