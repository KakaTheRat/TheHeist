#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnigmaComponent.h"
#include "Components/TextRenderComponent.h" // Important pour le texte 3D
#include "Keypad.generated.h"

UCLASS()
class THEHEIST_API APhysicalKeypad : public AActor
{
	GENERATED_BODY()
	
public:	
	APhysicalKeypad();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UEnigmaComponent* EnigmaComponent;

	// Le composant qui affiche le texte en 3D dans le monde
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTextRenderComponent* ScreenText;

	// Mémorise ce que le joueur tape
	FString CurrentCode;

public:	
	// Appelée par les boutons
	void ReceiveInput(FString Value);

private:
	void UpdateDisplay();
	void HandleValidation();
};