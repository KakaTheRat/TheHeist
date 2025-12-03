#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "PuzzleKeyPadWidget.generated.h"

/**
 * Widget for Puzzle KeyPad display and feedback
 */
UCLASS()
class PUZZLEKEYPAD_API UPuzzleKeyPadWidget : public UUserWidget
{
	GENERATED_BODY()

public :

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateDisplay(const FText& Text);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateVisualResult(bool bCodeIsRight);
};
