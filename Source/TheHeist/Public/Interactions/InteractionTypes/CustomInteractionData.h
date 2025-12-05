// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactions/InteractionTypes/InteractionData.h"
#include "CustomInteractionData.generated.h"

/**
 * 
 */
UCLASS()
class THEHEIST_API UCustomInteractionData : public UInteractionData
{
	GENERATED_BODY()

public:
	
	virtual void StartInteraction() override;
};
