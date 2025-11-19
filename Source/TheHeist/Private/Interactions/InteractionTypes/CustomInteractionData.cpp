// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactions/InteractionTypes/CustomInteractionData.h"

#include "BaseGizmos/GizmoElementShared.h"
#include "Interactions/InteractionInterface.h"

void UCustomInteractionData::StartInteraction()
{
	IInteractionInterface::Execute_Interact(Owner, Target, InteractingActor, EInteractionContext::Default, this);
}


