#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "PuzzleKeyPadWidget.generated.h"

/**
 * Widget for Puzzle KeyPad display and visual feedback
 * Handles UI presentation for the puzzle keypad
 */
UCLASS()
class PUZZLEKEYPAD_API UPuzzleKeyPadWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	/* Updates the display text in the widget */
	UFUNCTION(BlueprintImplementableEvent, Category = "Puzzle Widget")
	void UpdateDisplay(const FText& Text);

	/* Updates the visual feedback based on validation result */
	UFUNCTION(BlueprintImplementableEvent, Category = "Puzzle Widget")
	void UpdateVisualResult(bool bCodeIsRight);
};