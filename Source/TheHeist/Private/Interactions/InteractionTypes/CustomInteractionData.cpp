// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactions/InteractionTypes/CustomInteractionData.h"

#include "BaseGizmos/GizmoElementShared.h"
#include "Interactions/InteractionInterface.h"

void UCustomInteractionData::ExecuteInteraction(AActor* Owner, USceneComponent* Target, EInteractionContext Context, AActor* InteractingActor)
{
	Super::ExecuteInteraction(Owner, Target, Context, nullptr);
	IInteractionInterface::Execute_Interact(Owner, Target, InteractingActor, EInteractionContext::Default, this);
	
	
}


