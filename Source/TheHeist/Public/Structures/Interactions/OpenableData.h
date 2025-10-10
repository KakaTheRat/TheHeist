#pragma once

#include "CoreMinimal.h"
#include "InteractionData.h"
#include "Components/TimelineComponent.h"
#include "TimelinesComponent.h"
#include "Enumerators/Interactions/InteractionsEnum.h"
#include "OpenableData.generated.h"

UCLASS(Blueprintable, EditInlineNew, DisplayName="Opening Interaction")
class THEHEIST_API UOpenableData : public UInteractionData
{
	GENERATED_BODY()

public:

	virtual void Tick(float DeltaTime);
	
protected:

	UOpenableData();
	
	virtual void ExecuteInteraction(AActor* Owner, USceneComponent* Target) override;


	
protected:
	
	//------------Functions-----------//

	//Called each frame while the timeline is playing.
	UFUNCTION()
	void OnTimelineProgress(float Value);
	

	//Called whenever the timeline has stopped
	UFUNCTION()
	void HandleFinished();

	//Handles the actual movement or rotation.
	UFUNCTION()
	void HandleProgress(float Value);
	
	
	//-----------Properties-----------//

	
	//Allow to choose the type of opening wanted
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Opening")
	EOpenableType OpenableType = EOpenableType::Door;
	
	
	//Opening side for the object. Defines the direction of the whole movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Opening")
	EOpeningSide OpeningSide= EOpeningSide::Right;

	/*/ 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Openable")
	EOpeningDirection OpeningDirection = EOpeningDirection::Push;/*/

	//Opening progression (based of the timeline)
	UPROPERTY(BlueprintReadWrite)
	float OpenProgress = 0.f;

	//Chosen angle for the door opening
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Opening", meta=(EditCondition="OpeningSide==EOpeningSide::Door", EditConditionHides))
	float Angle = 90;

	//Opening distance for the drawer
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Opening", meta=(EditCondition="OpeningSide==EOpeningSide::Drawer", EditConditionHides))
	float Distance = 10;
	
	// Curve to control the animation
	UPROPERTY(EditAnywhere, Category="Opening")
	UCurveFloat* Curve;
        
	// Optional length of animation (curve can have its own length)
	UPROPERTY(EditAnywhere, Category="Opening")
	float Duration = 1.0f;
        
	bool bIsOpened = false;
	bool bIsMoving = false;

	FTimeline Timeline;
	
	bool bHasStoredInitialTransform;

	FVector InitialLocation;
	FRotator InitialRotation;
	
	bool bTimelineInitialized = false;
	void InitTimeline(AActor* Owner);
};
