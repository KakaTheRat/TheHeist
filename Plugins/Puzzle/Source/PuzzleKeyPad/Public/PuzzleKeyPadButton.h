// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "PuzzleKeyPadButton.generated.h"

UCLASS()
class PUZZLEKEYPAD_API APuzzleKeyPadButton : public AActor
{
	GENERATED_BODY()

public:	
	APuzzleKeyPadButton();

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ButtonMesh;

	// La valeur de cette touche (ex: "1", "C" pour Clear, "V" pour Valider)
	UPROPERTY(EditAnywhere, Category = "Config")
	FString ButtonValue;

	// Référence vers le clavier principal (à assigner dans l'éditeur)
	UPROPERTY(EditAnywhere, Category = "Config")
	APuzzleKeyPad* LinkedKeypad;

public:	
	// Cette fonction sera appelée par votre Personnage quand il clique dessus
	UFUNCTION(BlueprintCallable)
	void InterractWithButton();
};
