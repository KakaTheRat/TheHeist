// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWindowWidget.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionClicked, FString, InteractionName);

UCLASS()
class THEHEIST_API UInteractionWindowWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FOnInteractionClicked OnInteractionClicked;

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void TriggerInteraction(const FString m_InteractionName)
	{
		OnInteractionClicked.Broadcast(m_InteractionName);
	}
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable, Category="Interaction")
	void AddInteraction(const FString& m_InteractText);
	
	virtual void AddInteraction_Implementation(const FString& m_InteractText);
};
