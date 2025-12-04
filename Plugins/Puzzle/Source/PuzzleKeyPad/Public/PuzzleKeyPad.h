#pragma once

#include "CoreMinimal.h"
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
	
	/* Event called when the puzzle is successfully solved */
	UFUNCTION(BlueprintImplementableEvent, Category = "Puzzle Events")
	void OnPuzzleSolvedEvent();

	/* Event called when puzzle validation fails */
	UFUNCTION(BlueprintImplementableEvent, Category = "Puzzle Events")
	void OnPuzzleFailedEvent();
	
	/* Called when the puzzle is successfully solved */
	virtual void OnPuzzleSolved() override;

	/* Called when puzzle validation fails */
	virtual void OnPuzzleFailed() override;

private:
	/** The current entered code */
	UPROPERTY(VisibleAnywhere, Category = "Puzzle State")
	FString CurrentCode;

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
};