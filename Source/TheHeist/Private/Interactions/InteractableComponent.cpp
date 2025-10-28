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

// Called when the game starts
void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Owner = GetOwner();

	
	// Initialises the array for all the components used for an interaction.
	AttachedComponents.Empty();
	TArray<USceneComponent*> Components;
	Owner->GetComponents<USceneComponent>(Components);

	for (UInteractionData* Data : AllInteractions)
	{
		if (!Data || Data->CompNames.IsNone())
			continue;

		for (USceneComponent* Comp : Components)
		{
			if (Comp && Comp->GetFName() == Data->CompNames)
			{
				AllComponentInteractable.AddUnique(Comp);
				break;
			}
		}
	}
	
	for (USceneComponent* Comp : Components)
	{
		if (Comp)
		{
			AttachedComponents.Add(Comp);
		}
	}
}

// Called every frame
void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Allows Interactions Datas to tick (empty function overrided, means empty by default)
	for (UInteractionData* Data : AllInteractions)
	{
			if (Data)
			{
				Data->Tick(DeltaTime);
			
			}
	}
}


//Called at the registration, in editor mode (if not instancied in runtime)
void UInteractableComponent::OnRegister()
{
	Super::OnRegister();
	Owner = GetOwner();
}

#pragma region Interaction
FInteractionCascadeData* UInteractableComponent::FindValidCascade(const FString& m_InteractionText,EInteractionContext Context, const TSubclassOf<UInteractionData>& InteractionType,const UInteractionData* SpecificInteraction)
{
	for (FInteractionCascadeData& Cascade : InteractionsCascadeDatas)
	{
		if (Cascade.InteractionCascades.Num() == 0)
			continue;

		if (!Cascade.InteractionCascades.IsValidIndex(Cascade.MainSlotIndex))
			continue;

		// Checks context
		if (Cascade.ExpectedContext != Context && Cascade.ExpectedContext != EInteractionContext::Default)
			continue;

		// ----- Specific interaction searched//
		if (SpecificInteraction)
		{
			for (UInteractionCascadeSlot* Slot : Cascade.InteractionCascades)
			{
				if (Slot && Slot->InteractionData.Get() == SpecificInteraction)
				{
					return &Cascade;
				}
			}
			continue;
		}

		// ------- Interaction type OR interaction text searched
		UInteractionCascadeSlot* MainSlot = Cascade.InteractionCascades[Cascade.MainSlotIndex];
		if (!MainSlot || !MainSlot->InteractionData.IsValid())
			continue;

		UInteractionData* Data = MainSlot->InteractionData.Get();
		if (!Data)
			continue;

		// Text filter
		if (!m_InteractionText.IsEmpty() && Data->InteractText != m_InteractionText)
			continue;

		// Interaction type filter
		if (InteractionType && Data->GetClass() != InteractionType)
			continue;

		return &Cascade;
	}

	return nullptr;
}

//Execute interaction based on the interaction text. This will be used by the entity holding a player interaction component (mainly)
void UInteractableComponent::InteractWithObject(const FString m_InteractText, USceneComponent* HitComponent, AActor* InteractingActor, EInteractionContext Context)
{
	{
		if (!InteractingActor) return;

		//Checks for a cascade, for this type of interaction text. If so, will launch the cascade
		if (FInteractionCascadeData* Cascade = FindValidCascade(m_InteractText, Context, nullptr, nullptr))
		{
			Cascade->bIsComplete = false;
			ExecuteNextCascadeInteraction(*Cascade, InteractingActor, Context);
			return;
		}

		
		

		//Classic interaction
		{
			for (UInteractionData* Data : AllInteractions)
			{
				if (Data && HitComponent && Data->CompNames == HitComponent->GetName())
				{
					if (Data->InteractText ==m_InteractText)
					{
						Data->OnInteractionEnded.AddUObject(this, &UInteractableComponent::FinishInteraction);
						Data->ExecuteInteraction(Owner, HitComponent, Context, InteractingActor);
						break;
					}
				}
			}
		}
	}
}
	

void UInteractableComponent::InteractWithSpecificInteraction(TSubclassOf<UInteractionData> InteractionType,
	USceneComponent* HitComponent, AActor* InteractingActor, EInteractionContext Context,UInteractionData* InteractionInstance)
{
    if (!InteractingActor)
        return;

    USceneComponent* TargetComponent = HitComponent;

    //------------ Specific interaction requested//
    if (InteractionInstance)
    {
        // Trying to find a cascade fitting, will execute if so
        if (FInteractionCascadeData* Cascade = FindValidCascade(TEXT(""), Context, nullptr, InteractionInstance))
        {
            Cascade->bIsComplete = false;
            ExecuteNextCascadeInteraction(*Cascade, InteractingActor, Context);
            return;
        }

        // Searches for the component working with the given component
        if (!TargetComponent)
        {
            for (USceneComponent* Comp : AttachedComponents)
            {
                if (Comp && Comp->GetName() == InteractionInstance->CompNames)
                {
                    TargetComponent = Comp;
                    break;
                }
            }
        }

        if (TargetComponent)
        {
            InteractionInstance->OnInteractionEnded.AddUObject(this, &UInteractableComponent::FinishInteraction);
            InteractionInstance->ExecuteInteraction(Owner, TargetComponent, Context, InteractingActor);
        }

        return;
    }

    // -------Searches for an interaction, based of the type given.

    if (!TargetComponent)
    {
        for (USceneComponent* Comp : AttachedComponents)
        {
            if (!Comp)
                continue;

            for (UInteractionData* Data : AllInteractions)
            {
                if (Data && Data->GetClass() == InteractionType)
                {
                    TargetComponent = Comp;
                    break;
                }
            }

            if (TargetComponent)
                break;
        }
    }

    if (!TargetComponent)
        return;

    // Checks the cascade for the given type
    if (FInteractionCascadeData* Cascade = FindValidCascade(TEXT(""), Context, InteractionType, nullptr))
    {
        Cascade->bIsComplete = false;
        ExecuteNextCascadeInteraction(*Cascade, InteractingActor, Context);
        return;
    }

    // Executes the interaction, based of the type.
    for (UInteractionData* Data : AllInteractions)
    {
        if (Data && Data->GetClass() == InteractionType)
        {
            Data->OnInteractionEnded.AddUObject(this, &UInteractableComponent::FinishInteraction);
            Data->ExecuteInteraction(Owner, TargetComponent, Context, InteractingActor);
            break;
        }
    }
}

void UInteractableComponent::FinishInteraction(AActor* InteractingActor, UInteractionData* Interaction)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Interaction completed!")));

	//Event dispatcher, broadcasted when a simple interaction is finished or when a cascade is ended. For a cascade, will likely broadcast the MAIN INDEX interaction
	OnInteractionEndedEvent.Broadcast(InteractingActor, Interaction);
}

#pragma endregion


//---------------------------------Post edit, cascades and getters--------------------------------------------//

#pragma region Getters
TArray<FString> UInteractableComponent::GetInteractionsForAComp(USceneComponent* Comp)
{
	TArray<FString> Result;

	for (const UInteractionData* Data : AllInteractions)
	{
		if (Data->CompNames == Comp->GetName())
		{
			Result.Add(Data->InteractText);
		}
	}
	
	return Result;
}

TArray<FName> UInteractableComponent::GetAvailableInteractionComponents()
{
	TArray<FName> Components;
	return Components;
}

TArray<FName> UInteractableComponent::GetAvailableInteractionsForSelectedComponent()
{
	TArray<FName> Components;
	return Components;
}

TArray<FName> UInteractionCascadeSlot::GetAvailableInteractionComponents()
{
	TArray<FName> Names;

	//Searches for every interactable components. Will add each to the array
	if (!GetOuter()) return Names;

	if (UInteractableComponent* CompOwner = Cast<UInteractableComponent>(GetOuter()))
	{
		for (UInteractionData* Data : CompOwner->AllInteractions)
		{
			if (Data)
			{
				Names.AddUnique(Data->CompNames);
				
			}
		}
	}
	RefreshData();
	return Names;
}

TArray<FName> UInteractionCascadeSlot::GetAvailableInteractionsForSelectedComponent()
{
	TArray<FName> Names;
	if (!GetOuter()) return Names;

	//Searches for every interactions for a given component. Will add each to the array
 
	if (UInteractableComponent* CompOwner = Cast<UInteractableComponent>(GetOuter()))
	{
		for (UInteractionData* Data : CompOwner->AllInteractions)
		{
			if (!Data) continue;
			if (Data->CompNames == SelectedComponentName)
				Names.Add(FName(*Data->GetName()));
		}
	}
	RefreshData();
	return Names;
}

#pragma endregion

#pragma region PostEdit

#if WITH_EDITOR
void UInteractableComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
	
    if (PropertyChangedEvent.Property)
    {
        const FName PropertyName = PropertyChangedEvent.Property->GetFName();
        const FName MemberPropertyName = PropertyChangedEvent.MemberProperty ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;
    	
    }
}
#endif

#pragma endregion

#pragma region CascadeInteraction


TArray<FName> UInteractableComponent::GetAvailableCascadeNames()
{
	TArray<FName> Names;
	
	Names.Add(FName("New"));

	//Returns all cascades names
	
	for (const FInteractionCascadeData& CascadeData : InteractionsCascadeDatas)
	{
		if (!CascadeData.CascadeName.IsNone())
		{
			Names.Add(CascadeData.CascadeName);
		}
	}

	return Names;
}

void UInteractableComponent::ExecuteNextCascadeInteraction(FInteractionCascadeData& Cascade, AActor* InteractingActor, EInteractionContext Context)
{
	//Reset the cascade if the cycle is over
	if (!Cascade.InteractionCascades.IsValidIndex(Cascade.CurrentIndex))
	{
		Cascade.bIsComplete = true;
		Cascade.CurrentIndex = 0;
		UInteractionData* Interaction = Cascade.InteractionCascades[Cascade.MainSlotIndex]->InteractionData.Get();
		FinishInteraction(InteractingActor, Interaction);
		return;
	}
	UInteractionCascadeSlot* Slot = Cascade.InteractionCascades[Cascade.CurrentIndex];
	Cascade.CurrentIndex++;

	if (!Slot || !Slot->InteractionData.IsValid())
		return;

	UInteractionData* Interaction = Slot->InteractionData.Get();
	if (!Interaction)
		return;

	

	if (!Slot->ExpectedState.IsNone())
	{
		const FName CurrentState = Interaction->GetCurrentState();
		if (CurrentState == Slot->ExpectedState)
		{
			ExecuteNextCascadeInteraction(Cascade, InteractingActor, Context); 
			return;
		}
	}

	
	//Searches for the right component
	USceneComponent* TargetComp = nullptr;
	TArray<USceneComponent*> Components;
	Owner->GetComponents<USceneComponent>(Components);
	
	for (USceneComponent* Comp : Components)
	{
		if (Comp && Comp->GetName() == Interaction->CompNames)
		{
			TargetComp = Comp;
			break;
		}
	}

	if (!TargetComp)
		return;

	
	//Subscribes the cascade to the end of the interaction event. Will cause to execute the next interaction in this cascade, if possible
	FDelegateHandle Handle;
	Handle = Interaction->OnInteractionEnded.AddLambda(
		[this, CascadePtr = &Cascade, Context, &Handle](AActor* InteractingActor, UInteractionData* Interaction) mutable
		{
			Interaction->OnInteractionEnded.Remove(Handle);
			if (!CascadePtr->bIsComplete) 
			{
				ExecuteNextCascadeInteraction(*CascadePtr, InteractingActor, Context);
			}
		}
	);
	
	Interaction->ExecuteInteraction(Owner, TargetComp, Context, InteractingActor );
}



#pragma endregion

#pragma region CascadeStruct

TArray<FName> UInteractionCascadeSlot::GetAvailableStates() const
{
	TArray<FName> States;
//Will return every possible state for the given interaction
	if (InteractionData.IsValid())
	{
		States = InteractionData->GetAvailableStates();
	}
	else
	{
		States.Add("None");
	}

	return States;
}

void UInteractionCascadeSlot::RefreshData()
{
	#if WITH_EDITOR
	if (!GetOuter())
		return;

	
	UInteractableComponent* CompOwner = Cast<UInteractableComponent>(GetOuter());
	if (!CompOwner)
		return;
	
	if (!SelectedInteractionName.IsNone())
	{
		for (UInteractionData* Data : CompOwner->AllInteractions)
		{
			if (!Data)
				continue;
			
			if (Data->CompNames == SelectedComponentName && Data->GetName() == SelectedInteractionName.ToString())
			{
				InteractionData = Data;
				break;
			}
		}
	}

#endif
	
}

/*TArray<FString> FInteractionCascadeData::GetAvailableSlotIndices() const
{
	TArray<FString> Options;
	/*for (int32 i = 0; i < InteractionCascades.Num(); ++i)
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
}*/
#pragma endregion
