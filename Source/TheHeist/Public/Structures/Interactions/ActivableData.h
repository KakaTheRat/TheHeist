#pragma once

#include "CoreMinimal.h"
#include "InteractionData.h"
#include "UObject/NoExportTypes.h"
#include "Components/LightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Enumerators/Interactions/InteractionsEnum.h"
#include "ActivableData.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class THEHEIST_API UActivableData : public UInteractionData
{
	GENERATED_BODY()

public:

	UActivableData();
        
	//Light intensity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Activable")
	float LightIntensity = 1.f;
        
	//Activation state
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Activable")
	bool bIsActivated;
        
	//Allow to choose the type of activation wanted
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Activable")
	EActivableType ActivationType;
        
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Activable")
	USoundBase* Sound;

	virtual void ExecuteInteraction_Implementation(AActor* Owner, USceneComponent* Target) override;

protected:

	UFUNCTION()
	void ActivateLight(AActor* Owner);

	UFUNCTION()
	void ActivateSound(AActor* Owner);
};