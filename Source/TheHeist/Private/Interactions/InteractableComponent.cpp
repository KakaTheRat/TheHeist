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
/*
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
	}*/
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

//Called at the registration, in editor mode (if not instancied in runtime)
void UInteractableComponent::OnRegister()
{
	Super::OnRegister();
	Owner = GetOwner();
}

//Interaction interface interact function
void UInteractableComponent::Interact_Implementation(USceneComponent* HitComponent, AActor* InteractingActor, EInteractionContext Context)
{
	IInteractionInterface::Interact_Implementation(HitComponent, InteractingActor, Context);

/*
	if (!InteractingActor)return;




	InteractingActorr = InteractingActor;
	CurrentInteractionContext = Context;


	
	CurrentlyChosenComponent = HitComponent;

	//Checks if actor has the player's interaction component
	if (UPlayerInteractionComponent* PlayerInteraction = InteractingActor->FindComponentByClass<UPlayerInteractionComponent>())
	{
		// Get the player's widget actor
		AInteractionWidgetActor* WidgetActor = PlayerInteraction->GetInteractionWidget();

		if (!WidgetActor) return;
		
		WidgetActor->ClearEntries();

		for (const UInteractionData* Data : AllInteractions)
		{
			if (Data->CompNames == CurrentlyChosenComponent->GetName())
			{
				WidgetActor->AddInteractionEntry(Data->InteractText);
			}
		}
		/*/

	/*Add new entries to the interaction widget
		for (FInteractionEntry& Entry : InteractionsConfig)
		{
			
			if (Entry.ComponentName == CurrentlyChosenComponent->GetName())
			{
				CurrentEntry = &Entry;

				for (UInteractionData* Data : Entry.Interactions)
				{
					if (Data)
					{
						WidgetActor->AddInteractionEntry(Data->InteractText);
					}
				}
			}
		}
		
		WidgetActor->ShowWidget(HitComponent->GetComponentLocation()+ FVector(0, 0, 20));
		
		//Subscribe to the onclick event of the widget
		WidgetActor->GetWidget()->OnInteractionClicked.BindDynamic(this, &UInteractableComponent::InteractWithObject);
	}
	else
	{
		InteractWithObject("Hide");
	}
	//Checks if actor has the player's interaction component
	if (UPlayerInteractionComponent* PlayerInteraction = InteractingActor->FindComponentByClass<UPlayerInteractionComponent>())
	{
		
	}
	else
	{
	}*/
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
			//if (Data) Data->ExecuteInteraction(Owner, TargetComponent);
		}
	}
}

FInteractionCascadeData* UInteractableComponent::FindValidCascade(const FString& m_InteractionText,EInteractionContext Context, const TSubclassOf<UInteractionData>& InteractionType,UInteractionData* SpecificInteraction)
{
	for (FInteractionCascadeData& Cascade : InteractionsCascadeDatas)
	{
		if (Cascade.InteractionCascades.Num() == 0)
			continue;

		if (!Cascade.InteractionCascades.IsValidIndex(Cascade.MainSlotIndex))
			continue;

		// Vérifie le contexte
		if (Cascade.ExpectedContext != Context && Cascade.ExpectedContext != EInteractionContext::Default)
			continue;

		// Si on cherche une interaction spécifique :
		if (SpecificInteraction)
		{
			for (UInteractionCascadeSlot* Slot : Cascade.InteractionCascades)
			{
				if (Slot && Slot->InteractionData.Get() == SpecificInteraction)
				{
					return &Cascade; // ✅ trouvé
				}
			}
			continue; // pas dans cette cascade
		}

		// Sinon, logique habituelle : recherche par texte ou type
		UInteractionCascadeSlot* MainSlot = Cascade.InteractionCascades[Cascade.MainSlotIndex];
		if (!MainSlot || !MainSlot->InteractionData.IsValid())
			continue;

		UInteractionData* Data = MainSlot->InteractionData.Get();
		if (!Data)
			continue;

		// Filtrage par texte
		if (!m_InteractionText.IsEmpty() && Data->InteractText != m_InteractionText)
			continue;

		// Filtrage par type
		if (InteractionType && Data->GetClass() != InteractionType)
			continue;

		return &Cascade;
	}

	return nullptr;
}

//Execute interaction based on the type
void UInteractableComponent::InteractWithObject(const FString m_InteractText, USceneComponent* HitComponent, AActor* InteractingActor, EInteractionContext Context)
{
	{
		if (!InteractingActor) return;

		InteractingActorr = InteractingActor;
		CurrentInteractionContext = Context;
		CurrentlyChosenComponent = HitComponent;

		//Cascade interaction
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
				if (Data && CurrentlyChosenComponent && Data->CompNames == CurrentlyChosenComponent->GetName())
				{
					if (Data->InteractText ==m_InteractText)
					{
						Data->OnInteractionEnded.AddUObject(this, &UInteractableComponent::FinishInteraction);
						Data->ExecuteInteraction(Owner, CurrentlyChosenComponent, Context, InteractingActor);
						break;
					}
				}
			}
		}
	}
}
	/*
	
	if (!InteractingActor)return;

	InteractingActorr = InteractingActor;
	CurrentInteractionContext = Context;


	
	CurrentlyChosenComponent = HitComponent;
	
	bool bFoundCascade = false;

	for (FInteractionCascadeData& Cascade : InteractionsCascadeDatas)
	{
		if (Cascade.InteractionCascades.Num() == 0)
			continue;

		if (!Cascade.InteractionCascades.IsValidIndex(Cascade.MainSlotIndex))
			continue;

		UInteractionCascadeSlot* MainSlot = Cascade.InteractionCascades[Cascade.MainSlotIndex];
		if (!MainSlot || !MainSlot->InteractionData.IsValid())
			continue;

		UInteractionData* Data = MainSlot->InteractionData.Get();
		if (Data->InteractText == m_InteractText)
		{
			bFoundCascade = true;
			ExecuteNextCascadeInteraction(Cascade, nullptr, {});
			break;
		}
	}


	if (!bFoundCascade)
	{
		/*
		// Normal single interaction
		for (UInteractionData* Data : CurrentEntry->Interactions)
		{
			if (Data && Data->InteractText == m_InteractText)
			{
				Data->ExecuteInteraction(Owner, CurrentlyChosenComponent);
				break;
			}
		}*/

	/*
		for (UInteractionData* Data : AllInteractions)
		{
			if (Data->CompNames == CurrentlyChosenComponent->GetName())
			{
				if (Data->InteractText == m_InteractText)
				{
					Data->ExecuteInteraction(Owner, CurrentlyChosenComponent, Context, InteractingActorr);
					break;
				}
			}
		}
	}*/
	


void UInteractableComponent::InteractWithSpecificInteraction(TSubclassOf<UInteractionData> InteractionType,
	USceneComponent* HitComponent, AActor* InteractingActor, EInteractionContext Context,UInteractionData* InteractionInstance)
{
    if (!InteractingActor)
        return;

    USceneComponent* TargetComponent = HitComponent;

    // 🔹 1. Si on a une InteractionData spécifique, on regarde si elle appartient à une cascade
    if (InteractionInstance)
    {
        // Tente de trouver une cascade qui contient cette Interaction
        if (FInteractionCascadeData* Cascade = FindValidCascade(TEXT(""), Context, nullptr, InteractionInstance))
        {
            Cascade->bIsComplete = false;
            ExecuteNextCascadeInteraction(*Cascade, InteractingActor, Context);
            return;
        }

        // Sinon, exécution directe
        if (!TargetComponent)
        {
            // Cherche le bon composant à partir du nom enregistré dans l'interaction
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

    // 🔹 2. Aucun InteractionInstance → comportement standard

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

    // 🔹 3. Vérifie s’il existe une cascade pour ce type d’interaction
    if (FInteractionCascadeData* Cascade = FindValidCascade(TEXT(""), Context, InteractionType, nullptr))
    {
        Cascade->bIsComplete = false;
        ExecuteNextCascadeInteraction(*Cascade, InteractingActor, Context);
        return;
    }

    // 🔹 4. Interaction simple
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
	OnInteractionEndedEvent.Broadcast(InteractingActor, Interaction);
}
TArray<FName> UInteractionCascadeSlot::GetAvailableInteractionComponents()
{
	TArray<FName> Names;
	UE_LOG(LogTemp, Display, TEXT("GetAvailableIn"));
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
	UE_LOG(LogTemp, Display, TEXT("GetAvailableInteractionsForSelectedComponent"));
	if (!GetOuter()) return Names;
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



//---------------------------------Post edit and cascades--------------------------------------------//

#pragma region PostEdit

#if WITH_EDITOR
void UInteractableComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
	
    if (PropertyChangedEvent.Property)
    {
        const FName PropertyName = PropertyChangedEvent.Property->GetFName();
        const FName MemberPropertyName = PropertyChangedEvent.MemberProperty ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;

      /*  // Checks if its a property in the interaction cascade datas array
        if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(UInteractableComponent, InteractionsCascadeDatas))
        {
            // Refresh main slots
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
        }*/
    }
}
#endif

#pragma endregion

#pragma region CascadeInteraction

void UInteractableComponent::AddCascadeInteraction()
{
	/*if (SelectedComponentName.IsNone() || SelectedInteractionName.IsNone())
		return;

	FName TargetCascadeName = SelectedCascadeName;

	if (SelectedCascadeName == FName("New"))
	{
		if (NewCascadeName.IsNone())
			return;

		TargetCascadeName = NewCascadeName;
	}

	FInteractionCascadeData* ExistingCascade = InteractionsCascadeDatas.FindByPredicate(
		[TargetCascadeName](const FInteractionCascadeData& Cascade) { return Cascade.CascadeName == TargetCascadeName; });

	/*if (!ExistingCascade)
	{
		FInteractionCascadeData NewCascade;
		NewCascade.CascadeName = TargetCascadeName;
		InteractionsCascadeDatas.Add(NewCascade);
		ExistingCascade = &InteractionsCascadeDatas.Last();
	}

	UInteractionData* NewData = nullptr;

	/*for (FInteractionEntry& Entry : InteractionsConfig)
	{
		if (Entry.ComponentName == SelectedComponentName)
		{
			for (UInteractionData* Interaction : Entry.Interactions)
			{
				if (Interaction && Interaction->GetName() == SelectedInteractionName.ToString())
				{
					NewData = Interaction; 
					goto FoundData;
				}
			}
		}
	}
	for (UInteractionData* Data : AllInteractions)
	{
		if (Data->CompNames == SelectedComponentName)
		{
			if (Data->GetName() == SelectedInteractionName.ToString())
				{
					NewData = Data; 
					goto FoundData;
				}
			
		}
	}
	FoundData:
	if (!NewData)
		return;
	/*
	USceneComponent* TargetComp = nullptr;
	TArray<USceneComponent*> Components;
	Owner->GetComponents<USceneComponent>(Components);
	
	for (USceneComponent* Comp : Components)
	{
		if (Comp && Comp->GetName() == SelectedComponentName.ToString())
		{
			TargetComp = Comp;
			break;
		}
	}
	
	UInteractionCascadeSlot* NewSlot = NewObject<UInteractionCascadeSlot>(this);
	NewSlot->ComponentName = SelectedComponentName;
	NewSlot->InteractionData = NewData;
	InteractionsCascadeDatas[0].InteractionCascades.Add(NewSlot);*/
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

void UInteractableComponent::ExecuteNextCascadeInteraction(FInteractionCascadeData& Cascade, AActor* InteractingActor, EInteractionContext Context)
{
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

	
	
	FDelegateHandle Handle;
	Handle = Interaction->OnInteractionEnded.AddLambda(
		[this, CascadePtr = &Cascade, Context, &Handle](AActor* InteractingActor, UInteractionData* Interaction) mutable
		{
			Interaction->OnInteractionEnded.Remove(Handle);
			if (!CascadePtr->bIsComplete)  // ← Vérifier si terminé
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

	if (InteractionData.IsValid())
	{
		States = InteractionData->GetAvailableStates();
		UE_LOG(LogTemp, Display, TEXT("%s"), *InteractionData.Get()->GetName());
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
