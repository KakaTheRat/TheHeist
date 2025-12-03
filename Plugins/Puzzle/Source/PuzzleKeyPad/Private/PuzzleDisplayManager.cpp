#include "PuzzleDisplayManager.h"

#pragma region Initialization

UPuzzleDisplayManager::UPuzzleDisplayManager() : PlaceholderText("----")
{
	PrimaryComponentTick.bCanEverTick = false;
}

/**
 * Initializes the display manager with the given widget component
 * 
 * @param Widget The widget component for UI display
 */
void UPuzzleDisplayManager::Initialize(UWidgetComponent* Widget)
{
	WidgetComponent = Widget;

	if (WidgetComponent)
	{
		WidgetInstance = Cast<UPuzzleKeyPadWidget>(WidgetComponent->GetWidget());
	}
}

#pragma endregion

#pragma region Public Display Methods

/**
 * Updates the display with the current code
 * 
 * @param Code The current code to display
 */
void UPuzzleDisplayManager::UpdateDisplay(const FString& Code)
{
	const FString DisplayText = Code.IsEmpty() ? PlaceholderText : Code;
	
	UpdateWidgetDisplay(DisplayText);
}

/**
 * Shows validation result on the display
 * 
 * @param bSuccess true if validation succeeded, false otherwise
 */
void UPuzzleDisplayManager::ShowValidationResult(bool bSuccess)
{
	UpdateWidgetVisualResult(bSuccess);
}

/**
 * Resets the display to default state
 */
void UPuzzleDisplayManager::ResetDisplay()
{
	UpdateDisplay("");
}

#pragma endregion

#pragma region Widget Communication

/**
 * Updates the widget display
 * 
 * @param Text The text to display in the widget
 */
void UPuzzleDisplayManager::UpdateWidgetDisplay(const FString& Text) const
{
	if (WidgetInstance)
	{
		WidgetInstance->UpdateDisplay(FText::FromString(Text));
	}
}

/**
 * Updates the widget visual feedback
 * 
 * @param bSuccess true if validation succeeded, false otherwise
 */
void UPuzzleDisplayManager::UpdateWidgetVisualResult(bool bSuccess) const
{
	if (WidgetInstance)
	{
		WidgetInstance->UpdateVisualResult(bSuccess);
	}
}

#pragma endregion