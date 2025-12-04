#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KeypadButton.generated.h"

// Forward declaration pour éviter les boucles d'include
class APhysicalKeypad; 

UCLASS()
class THEHEIST_API AKeypadButton : public AActor
{
	GENERATED_BODY()
	
public:	
	AKeypadButton();

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ButtonMesh;

	// La valeur de cette touche (ex: "1", "C" pour Clear, "V" pour Valider)
	UPROPERTY(EditAnywhere, Category = "Config")
	FString ButtonValue;

	// Référence vers le clavier principal (à assigner dans l'éditeur)
	UPROPERTY(EditAnywhere, Category = "Config")
	APhysicalKeypad* LinkedKeypad;

public:	
	// Cette fonction sera appelée par votre Personnage quand il clique dessus
	UFUNCTION(BlueprintCallable)
	void InterractWithButton();
};