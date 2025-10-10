// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Interaction/InteractionWindowWidget.h"

void UInteractionWindowWidget::AddInteraction_Implementation(const FString& m_InteractText)
{
}
void UInteractionWindowWidget::TriggerInteraction(const FString m_InteractionName)
{
	OnInteractionClicked.ExecuteIfBound(m_InteractionName);
}