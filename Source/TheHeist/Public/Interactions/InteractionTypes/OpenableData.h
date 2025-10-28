#pragma once

#include "CoreMinimal.h"
#include "InteractionData.h"
#include "Components/TimelineComponent.h"
#include "Enumerators/Interactions/InteractionsEnum.h"
#include "OpenableData.generated.h"


UENUM(BlueprintType)
enum class EOpeningStates : uint8
{
	Open     UMETA(DisplayName="Open"),
	Close    UMETA(DisplayName="Closed"),
};

UCLASS(Blueprintable, EditInlineNew, DisplayName="Opening Interaction")
class THEHEIST_API UOpenableData : public UInteractionData
{
	GENERATED_BODY()

public:


virtual FName GetCurrentState_Implementation() const override;
	
	
	virtual void Tick(float DeltaTime);

	virtual TArray<FName> GetAvailableStates() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EOpeningStates CurrentState;
	
protected:

	UOpenableData();
	
	virtual void ExecuteInteraction(AActor* Owner, USceneComponent* Target, EInteractionContext Context, AActor* InteractingActor) override;


	
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
	EOpeningType OpenableType = EOpeningType::Door;


	//Actives or deactivates the opening side choice (for a simple pulled drawer, for exemple, won't need)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Opening")
	bool b_ShouldUseOpeningSide = false;
	
	//Opening side for the object. Defines the direction side of the whole movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Opening",meta=(EditCondition="b_ShouldUseOpeningSide", EditConditionHides))
	EOpeningSide OpeningSide= EOpeningSide::Right;
	
	//Opening Direction for the object. Defines the direction of the whole movement (will define if must be pushed or pulled when interacted with)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Opening")
	EOpeningDirection OpeningDirection = EOpeningDirection::Push;

	
	
	//Opening progression (based of the timeline)
	UPROPERTY(BlueprintReadWrite)
	float OpenProgress = 0.f;

	//Chosen angle for the door opening
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Opening", meta=(EditCondition="OpenableType==EOpeningType::Door", EditConditionHides))
	float Angle = 90;

	//Opening distance for the drawer
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Opening", meta=(EditCondition="OpenableType==EOpeningType::Drawer", EditConditionHides))
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
	
	bool bHasStoredInitialTransform = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Opening")
	FVector InitialLocation;
	FRotator InitialRotation;
	
	bool bTimelineInitialized = false;
	void InitTimeline(AActor* Owner);
};
