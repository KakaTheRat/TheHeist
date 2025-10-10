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
	WidgetInstance->MyWidget->OnInteractionClicked.BindDynamic(this, &UInteractableComponent::InteractWithObject);
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
	bool bFoundCascade = false;

	for (FInteractionCascadeData& Cascade : InteractionsCascadeDatas)
	{
		if (Cascade.InteractionCascades.Num() > 0 && Cascade.InteractionCascades[0].InteractionData)
		{
			if (Cascade.InteractionCascades[0].InteractionData->InteractText == m_InteractText)
			{
				bFoundCascade = true;
				CurrentCascade = &Cascade;
				CurrentCascadeIndex = 0;
				ExecuteNextCascadeInteraction(); // Launch next interaction
				break;
			}
		}
	}

	if (!bFoundCascade)
	{
		// Interaction normale (hors cascade)
		for (UInteractionData* Data : CurrentEntry->Interactions)
		{
			if (Data && Data->InteractText == m_InteractText)
			{
				Data->ExecuteInteraction(Owner, CurrentlyChosenComponent);
				break;
			}
		}
	}

	// Détruit le widget
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

TArray<FName> UInteractableComponent::GetAvailableInteractionComponents()
{
	TArray<FName> ComponentNames;

	for (const FInteractionEntry& Entry : InteractionsConfig)
	{
		if (Entry.Interactions.Num() > 0 && !ComponentNames.Contains(Entry.ComponentName))
		{
			ComponentNames.Add(Entry.ComponentName);
		}
	}

	return ComponentNames;
}

TArray<FName> UInteractableComponent::GetAvailableInteractionsForSelectedComponent()
{
	TArray<FName> InteractionNames;

	if (SelectedComponentName.IsNone())
		return InteractionNames;

	for (const FInteractionEntry& Entry : InteractionsConfig)
	{
		if (Entry.ComponentName == SelectedComponentName)
		{
			for (UInteractionData* Interaction : Entry.Interactions)
			{
				if (Interaction)
				{
					InteractionNames.Add(FName(*Interaction->GetName()));
				}
			}
		}
	}

	return InteractionNames;
}




// ---------------------------------------------------------------
// Debug helper to test directly in editor
// ---------------------------------------------------------------

void UInteractableComponent::AddCascadeInteraction()
{
	if (SelectedComponentName.IsNone() || SelectedInteractionName.IsNone())
	{
		return;
	}

	FName TargetCascadeName = SelectedCascadeName;
	
	if (SelectedCascadeName == FName("New"))
	{
		if (NewCascadeName.IsNone())
		{
			return;
		}
		TargetCascadeName = NewCascadeName;
	}

	FInteractionCascadeData* ExistingCascade = nullptr;

	for (FInteractionCascadeData& Cascade : InteractionsCascadeDatas)
	{
		if (Cascade.CascadeName == TargetCascadeName)
		{
			ExistingCascade = &Cascade;
			break;
		}
	}

	if (!ExistingCascade)
	{
		FInteractionCascadeData NewCascade;
		NewCascade.CascadeName = TargetCascadeName;
		InteractionsCascadeDatas.Add(NewCascade);
		ExistingCascade = &InteractionsCascadeDatas.Last();
	}
	
	FInteractionEntry* NewEntry = nullptr;

	UInteractionData* NewData = nullptr;
	
	for (FInteractionEntry& Entry : InteractionsConfig)
	{
		if (Entry.ComponentName == SelectedComponentName)
		{
			for (UInteractionData* Interaction : Entry.Interactions)
			{
				if (Interaction && Interaction->GetName() == SelectedInteractionName.ToString())
				{
					NewEntry = &Entry;
					NewData = Interaction;
					
					break;
				}
			}
		}
	}
	
	for (FInteractionCascadeSlot& Slot : ExistingCascade->InteractionCascades)
	{
		if (Slot.InteractionData == NewData)
		{
			if (Slot.ComponentName == NewEntry->ComponentName)
			{
				return;
			}
			
		}
	}

	FInteractionCascadeSlot NewCascade;
	NewCascade.InteractionData = NewData;
	NewCascade.ComponentName = SelectedComponentName;
	ExistingCascade->InteractionCascades.Add(NewCascade);
}

TArray<FName> UInteractableComponent::GetAvailableCascadeNames()
{
	TArray<FName> Names;
	
	Names.Add(FName("New"));
	
	for (const FInteractionCascadeData& CascadeData : InteractionsCascadeDatas)
	{
		if (!CascadeData.CascadeName.IsNone())
		{
			Names.Add(CascadeData.CascadeName);
		}
	}

	return Names;
}

void UInteractableComponent::ExecuteNextCascadeInteraction()
{
	if (!CurrentCascade) return;
    
	if (!CurrentCascade->InteractionCascades.IsValidIndex(CurrentCascadeIndex)) 
	{
		CurrentCascade = nullptr;
		CurrentCascadeIndex = 0;
		return;
	}

	FInteractionCascadeSlot& Slot = CurrentCascade->InteractionCascades[CurrentCascadeIndex];
    
	// D'abord incrémenter l'index
	CurrentCascadeIndex++;
    
	if (Slot.InteractionData)
	{
		// Lier le callback pour l'interaction SUIVANTE
		Slot.InteractionData->OnInteractionEnded.BindUObject(this, &UInteractableComponent::ExecuteNextCascadeInteraction);
		Slot.InteractionData->ExecuteInteraction(Owner, CurrentlyChosenComponent);
	}
}

TArray<FString> FInteractionCascadeData::GetAvailableSlotIndices() const
{
	TArray<FString> Options;
	for (int32 i = 0; i < InteractionCascades.Num(); ++i)
	{
		const FInteractionCascadeSlot& Slot = InteractionCascades[i];
		
		FString Label = FString::Printf(TEXT("[%d] %s (%s)"),
			i,
			*Slot.ComponentName.ToString(),
			*GetNameSafe(Slot.InteractionData)
		);

		Options.Add(Label);
	}
	return Options;
}

void FInteractionCascadeData::RefreshMainSlot()
{
	if (InteractionCascades.IsValidIndex(MainSlotIndex))
	{
		MainSlot = InteractionCascades[MainSlotIndex]; 
	}
	else
	{
		MainSlot = FInteractionCascadeSlot();
	}
}
#if WITH_EDITOR
void UInteractableComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.Property)
    {
        const FName PropertyName = PropertyChangedEvent.Property->GetFName();
        const FName MemberPropertyName = PropertyChangedEvent.MemberProperty ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;

        // Vérifier si c'est une propriété dans FInteractionCascadeData
        if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(UInteractableComponent, InteractionsCascadeDatas))
        {
            // Forcer le refresh de tous les MainSlot
            for (FInteractionCascadeData& Cascade : InteractionsCascadeDatas)
            {
                Cascade.RefreshMainSlot();
            }
        }
        // Vérifier directement si MainSlotIndex a changé
        else if (PropertyName == GET_MEMBER_NAME_CHECKED(FInteractionCascadeData, MainSlotIndex))
        {
            // Trouver la cascade modifiée
            if (PropertyChangedEvent.GetArrayIndex(PropertyChangedEvent.Property->GetFName().ToString()) != INDEX_NONE)
            {
                int32 ArrayIndex = PropertyChangedEvent.GetArrayIndex(PropertyChangedEvent.Property->GetFName().ToString());
                if (InteractionsCascadeDatas.IsValidIndex(ArrayIndex))
                {
                    InteractionsCascadeDatas[ArrayIndex].RefreshMainSlot();
                }
            }
        }
    }
}
#endif

