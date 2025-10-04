#pragma once

#include "CoreMinimal.h"
#include "InteractionData.h"
#include "Components/TimelineComponent.h"
#include "TimelinesComponent.h"
#include "Enumerators/Interactions/InteractionsEnum.h"
#include "OpenableData.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class THEHEIST_API UOpenableData : public UInteractionData
{
	GENERATED_BODY()

public:

	UOpenableData();

        
	//Allow to choose the type of opening wanted
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Openable")
	EOpenableType OpenableType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Openable")
	bool bIsOpened = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Openable")
	float OpenProgress = 0.f;
        
	// Curve to control the animation
	UPROPERTY(EditAnywhere, Category="Openable")
	UCurveFloat* Curve;
        
	// Optional length of animation (curve can have its own length)
	UPROPERTY(EditAnywhere, Category="Openable")
	float Duration = 1.0f;
        
        
	virtual void ExecuteInteraction_Implementation(AActor* Owner, USceneComponent* Target) override;
	void HandleProgress(float Value);

	private :

		UFUNCTION()
	void OnTimelineProgress(float Value);
        
	void PlayForward();
	void PlayReverse();

	bool bManualAnimating = false;
	float ManualProgress = 0.0f;
	bool bManualReverse = false;
        
};