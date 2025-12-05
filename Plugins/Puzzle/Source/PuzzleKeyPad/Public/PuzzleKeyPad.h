#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "GameFramework/Actor.h"

#include "Components/WidgetComponent.h"
#include "PuzzleComponent.h"
#include "PuzzleDisplayManager.h"

#include "PuzzleKeyPad.generated.h"

/**
 * Puzzle KeyPad Actor
 * Orchestrates puzzle components using composition principle
 * Implements the Observer interface to react to puzzle events
 */
UCLASS()
class PUZZLEKEYPAD_API APuzzleKeyPad : public AActor, public IPuzzleObserver
{
	GENERATED_BODY()

public:
	
	APuzzleKeyPad();

	/* Entry point for keypad interactions */
	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	void ReceiveInput(FString Value);

	/* Clears the current code */
	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	void ClearCode();

	/* Sets the puzzle solution */
	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	void SetPuzzleSolution(const FString& Solution);

	/* Gets the puzzle component */
	UFUNCTION(BlueprintPure, Category = "Puzzle")
	UPuzzleComponent* GetPuzzleComponent() const { return PuzzleComponent; }

	/* Gets the current code */
	UFUNCTION(BlueprintPure, Category = "Puzzle")
	FString GetCurrentCode() const { return CurrentCode; }

protected:
	
	virtual void BeginPlay() override;
	
	/** Manages puzzle validation logic */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPuzzleComponent* PuzzleComponent;

	/** Manages display presentation */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPuzzleDisplayManager* DisplayManager;

	/** Widget component for UI display */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* ScreenWidget;
	
	/** Duration to display error message before reset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle Settings")
	float ErrorDisplayDuration;

	/** Keys array (jsp quoi dire d'autre mais lucie va me frapper si je commente pas) */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Puzzle State")
	TArray<UStaticMeshComponent*> Keys;

	/** The string used to delete the last character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	FString DeleteString;

	/** The current entered code */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Puzzle State")
	int CurrentKeyIndex;
	
	/* Event called when the puzzle is successfully solved */
	UFUNCTION(BlueprintImplementableEvent, Category = "Puzzle Events")
	void OnPuzzleSolvedEvent();

	/* Event called when puzzle validation fails */
	UFUNCTION(BlueprintImplementableEvent, Category = "Puzzle Events")
	void OnPuzzleFailedEvent();

	/** Called from Input Actions for interaction input */
	UFUNCTION(BlueprintImplementableEvent, Category = "Puzzle Events")
	void InteractWithKey(const FInputActionValue& Value);

	/** Setup the keypad session when interacted with */
	UFUNCTION(BlueprintCallable)
	void StartKeypadInteraction(AActor* Interactor);
	
	/** Called from Input Actions for movement input */
	void MoveAround(const FInputActionValue& Value);
	
	/* Called when the puzzle is successfully solved */
	virtual void OnPuzzleSolved() override;

	/* Called when puzzle validation fails */
	virtual void OnPuzzleFailed() override;

	//User Info//
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AActor* User = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent*  UserSkeletalMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	APlayerController* UserController = nullptr;


private:
	/** The current entered code */
	UPROPERTY(VisibleAnywhere, Category = "Puzzle State")
	FString CurrentCode;
	
	/** The current entered code */
	UPROPERTY(EditAnywhere, Category = "Puzzle")
	int rows;

	/** The current entered code */
	UPROPERTY(EditAnywhere, Category = "Puzzle")
	int columns;

	/** Overlay material instance for key highlight */
	UPROPERTY(EditAnywhere, Category = "Puzzle")
	UMaterialInstance* KeyMaterialInstance;
	
	/** Timer handle for error message reset */
	FTimerHandle ErrorResetTimerHandle;

	/* Initializes all components */
	void InitializeComponents();

	/* Appends a value to the current code */
	void AppendToCode(const FString& Value);

	/* Checks if the code is complete */
	bool IsCodeComplete() const;

	/* Updates the display with the current code */
	void UpdateDisplay();

	/* Requests validation of the current code */
	void RequestValidation();

	/* Resets the display after showing an error */
	void ResetAfterError();

	/* Modifies the keys material */
	void ManageMaterial(int KeyIndex,bool bShouldActivate);

	//Inputs//

	/** Inputs to move around keys */
	UPROPERTY(EditAnywhere, Category = "Puzzle State")
	UInputAction* MoveAroundAction;

	/** Inputs to move around keys */
	UPROPERTY(EditAnywhere, Category = "Puzzle State")
	UInputAction* InteractAction;

	/** Input mapping context for keypad interactions */
	UPROPERTY(EditAnywhere, Category = "Puzzle State")
	UInputMappingContext* KeyPadInputContext;

	/**Activates the keypad input and changes the input mapping */
	void ActivateKeyPadInput(bool bShouldActivate);

	/** Sets up input bindings */
	void SetupInputs();
	
	UPROPERTY(EditAnywhere, Category = "Input")
	int32 KeyPadInputPriority = 1;

	/** Animation montage for key interaction */
	UPROPERTY(EditAnywhere, Category = "Input")
	UAnimMontage* KeyInteractionMontage;

	/** Sound played on key interaction */
	UPROPERTY(EditAnywhere, Category = "Input")
	USoundBase* InteractionSound;

	/** Name of the notify in the animation montage to trigger interaction */
	UPROPERTY(EditAnywhere, Category = "Input")
	FName InteractionNotifyName;
	
 	/** Stores input binding handles for cleanup */
	TArray<uint32> InputBindingHandles;
	
	float LastMoveTime = 0.f;

	UPROPERTY(EditAnywhere, Category = "KeyPad")
	float MoveCooldown = 0.2f;
	
	bool bFirst = true;
};