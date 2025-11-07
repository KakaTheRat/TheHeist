#pragma once

#include "CoreMinimal.h"
#include "InteractionData.h"
#include "UObject/NoExportTypes.h"
#include "Components/LightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Datas/Enumerators/Interactions/InteractionsEnum.h"
#include "ActivableData.generated.h"

UCLASS(Blueprintable, EditInlineNew, DisplayName="Activating Interaction")
class THEHEIST_API UActivableData : public UInteractionData
{
	GENERATED_BODY()

public:

	//---------Functions--------------//
	
	UActivableData();

	virtual void ExecuteInteraction(AActor* Owner, USceneComponent* Target, EInteractionContext Context, AActor* InteractingActor) override;

	virtual TArray<FName> GetAvailableStates() override;


	//----------Properties--------------//

	//Activation state
	UPROPERTY(BlueprintReadWrite)
	bool bIsActivated;
        
	//Allow to choose the type of activation wanted
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Activable")
	EActivableType ActivationType;
	
	
	//--------Light related properties--------------//
	
	//Light intensity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Activable",meta=(EditCondition="ActivationType==EActivableType::Light", EditConditionHides))
	float LightIntensity = 1.f;
	

	//-----------Sound related properties--------------//

	//Sound asset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Activable",meta=(EditCondition="ActivationType==EActivableType::Sound", EditConditionHides))
	USoundBase* Sound;

	

protected:

	//Activation for the light
	UFUNCTION()
	void ActivateLight(const AActor* Owner);

	//Activation for the sound
	UFUNCTION()
	void ActivateSound(const AActor* Owner);
};