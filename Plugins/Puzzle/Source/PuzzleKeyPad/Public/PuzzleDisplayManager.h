#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Components/WidgetComponent.h"
#include "PuzzleKeyPadWidget.h"

#include "PuzzleDisplayManager.generated.h"

/**
 * Manages puzzle display presentation 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PUZZLEKEYPAD_API UPuzzleDisplayManager : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UPuzzleDisplayManager();

	/* Initializes the display manager with widget component */
	void Initialize(UWidgetComponent* Widget);

	/* Updates the display with the current code */
	UFUNCTION(BlueprintCallable, Category = "Puzzle Display")
	void UpdateDisplay(const FString& Code);

	/* Shows validation result */
	UFUNCTION(BlueprintCallable, Category = "Puzzle Display")
	void ShowValidationResult(bool bSuccess);

	/* Resets the display to default state */
	UFUNCTION(BlueprintCallable, Category = "Puzzle Display")
	void ResetDisplay();

protected:
	
	/** Reference to the widget component */
	UPROPERTY()
	UWidgetComponent* WidgetComponent;

	/** Reference to the widget instance */
	UPROPERTY()
	UPuzzleKeyPadWidget* WidgetInstance;

	/** Default placeholder text when no code is entered */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display Settings")
	FString PlaceholderText;

private:

	/* Updates the widget display */
	void UpdateWidgetDisplay(const FString& Text) const;
	
	/* Updates the widget visual feedback */
	void UpdateWidgetVisualResult(bool bSuccess) const;
};