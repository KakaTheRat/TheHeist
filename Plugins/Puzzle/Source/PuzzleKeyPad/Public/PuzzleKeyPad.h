// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KeyPadWidget.h"
#include "GameFramework/Actor.h"

#include "PuzzleComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"

#include "PuzzleKeyPad.generated.h"

UCLASS()
class PUZZLEKEYPAD_API APuzzleKeyPad : public AActor
{
	GENERATED_BODY()
	
public:	
	APuzzleKeyPad();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPuzzleComponent* PuzzleComponent;

	// Le composant qui affiche le texte en 3D dans le monde
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTextRenderComponent* ScreenText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* ScreenWidget;
	
	UPROPERTY()
	UKeyPadWidget* ScreenWidgetInstance;


	// Mémorise ce que le joueur tape
	FString CurrentCode;

public:	
	// Appelée par les boutons
	UFUNCTION(BlueprintCallable)
	void ReceiveInput(FString Value);

private:
	void UpdateDisplay();
	void HandleValidation();
};
