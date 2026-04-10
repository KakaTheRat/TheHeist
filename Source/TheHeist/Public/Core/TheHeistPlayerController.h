// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TheHeistPlayerController.generated.h"

class AMonitorActor;
class ASurveillanceRoomActor;
class UInputAction;
class UInputMappingContext;
class UUserWidget;

/**
 *  Simple first person Player Controller
 *  Manages the input mapping context.
 *  Overrides the Player Camera Manager class.
 */
UCLASS(abstract)
class THEHEIST_API ATheHeistPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	/** Constructor */
	ATheHeistPlayerController();

	UPROPERTY(EditAnywhere, Category = "Input|Monitor")
	UInputMappingContext* MonitorMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input|Monitor")
	UInputAction* UnfocusMonitorAction;

	UFUNCTION(BlueprintCallable, Category = "Monitor")
	void EnableMonitorInput();

	UFUNCTION(BlueprintCallable, Category = "Monitor")
	void DisableMonitorInput();

	UPROPERTY(EditAnywhere, Category = "Input|Monitor")
	UInputAction* SwipeNextAction;

	UPROPERTY(EditAnywhere, Category = "Input|Monitor")
	UInputAction* SwipePreviousAction;

	UPROPERTY()
	ASurveillanceRoomActor* CurrentRoom = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input|Monitor")
	float MonitorFocusLerpSpeed = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Input|Monitor")
	float MonitorFocusDistance = 80.0f;

private:
	
	UFUNCTION()
	void OnUnfocusMonitor();

	UFUNCTION()
	void OnSwipeNext();

	UFUNCTION()
	void OnSwipePrevious();
	
	FTransform OriginalCameraTransform;

	FTransform TargetCameraTransform;

	bool bIsFocusing = false;
	bool bIsUnfocusing = false;

	UPROPERTY()
	AMonitorActor* FocusedMonitorRef = nullptr;

	void StartFocusCamera(AMonitorActor* Monitor);
	void StartUnfocusCamera();
	void TickFocusCamera(float DeltaTime);

	virtual void PlayerTick(float DeltaTime) override;
	
	bool bOriginalUsePawnControlRotation = true;

	TArray<UInputMappingContext*> DisabledContextsDuringFocus;
	
protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

};
