// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KeyPadWidget.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEKEYPAD_API UKeyPadWidget : public UUserWidget
{
	GENERATED_BODY()

public :

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateDisplay(const FText& Text);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateVisualResult(bool bCodeIsRight);
};
