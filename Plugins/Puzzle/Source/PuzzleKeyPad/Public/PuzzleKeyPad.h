#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "PuzzleComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "PuzzleKeyPadWidget.h"

#include "PuzzleKeyPad.generated.h"

/**
 * Puzzle KeyPad Actor
 * Handles input from a keypad and checks against a puzzle solution.
 */
UCLASS()
class PUZZLEKEYPAD_API APuzzleKeyPad : public AActor
{
	GENERATED_BODY()
	
public:
	
	APuzzleKeyPad();

protected:
	
	virtual void BeginPlay() override;

	/* The puzzle component that handles solution checking */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPuzzleComponent* PuzzleComponent;

	/* Visual display components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTextRenderComponent* ScreenText;

	/* Widget component for advanced UI */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* ScreenWidget;

	/* Instance of the widget */
	UPROPERTY()
	UPuzzleKeyPadWidget* ScreenWidgetInstance;

	/* Current input code */
	FString CurrentCode;

public:
	
	/* Function to receive input from the keypad */
	UFUNCTION(BlueprintCallable)
	void ReceiveInput(FString Value);

private:
	
	/* Update the visual display */
	void UpdateDisplay() const;
	
	/* Handle validation of the entered code */
	void HandleValidation();
};
