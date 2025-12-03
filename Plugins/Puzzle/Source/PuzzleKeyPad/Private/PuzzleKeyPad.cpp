#include "PuzzleKeyPad.h"

#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"


APuzzleKeyPad::APuzzleKeyPad()
{
	PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	PuzzleComponent = CreateDefaultSubobject<UPuzzleComponent>(TEXT("PuzzleComponent"));

	ScreenText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ScreenText"));
	ScreenText->SetupAttachment(RootComponent);
    ScreenText->SetHorizontalAlignment(EHTA_Center);
    ScreenText->SetText(FText::FromString("----"));
    ScreenText->SetWorldSize(20.0f);

	ScreenWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ScreenWidget"));
	ScreenWidget->SetupAttachment(RootComponent);
}

void APuzzleKeyPad::BeginPlay()
{
	Super::BeginPlay();
	CurrentCode = "";

	if (ScreenWidget) ScreenWidgetInstance = Cast<UPuzzleKeyPadWidget>(ScreenWidget->GetWidget());
	
    UpdateDisplay();
}

/*
 * Receives input from the keypad buttons.
 * 
 * @param Value The input value from the keypad button.
 */
void APuzzleKeyPad::ReceiveInput(FString Value)
{
	//Clear the code
    if(Value == "C") 
    {
        CurrentCode = "";
        UpdateDisplay();
        return;
    }

    // Add the input value to the current code
	CurrentCode.Append(Value);
	UpdateDisplay();

    // Check if we need to validate the solution
	if (PuzzleComponent && PuzzleComponent->NeedToCheckSolution(CurrentCode))
	{
		HandleValidation();
	}
}

/*
 * Updates the visual display of the keypad with the current code.
 */
void APuzzleKeyPad::UpdateDisplay() const
{
	/* Update the TextRenderComponent display */
    if(ScreenText)
    {
        ScreenText->SetText(FText::FromString(CurrentCode));
    }

	/* Update the Widget display */
	if (ScreenWidget)
	{
		ScreenWidgetInstance->UpdateDisplay(FText::FromString(CurrentCode));
	}
}

/*
 * Handles the validation of the entered code against the puzzle solution.
 */
void APuzzleKeyPad::HandleValidation()
{
	bool bSuccess = PuzzleComponent->TrySolvePuzzle(CurrentCode);
	
	ScreenWidgetInstance->UpdateVisualResult(bSuccess);

	// Update the TextRenderComponent based on success or failure
	if (bSuccess)
	{
		ScreenText->SetTextRenderColor(FColor::Green);
		ScreenText->SetText(FText::FromString("OPEN"));
		
        // TODO : Ouvrir la porte ici ou appeler un Event
	}
	else
	{
		ScreenText->SetTextRenderColor(FColor::Red);
		ScreenText->SetText(FText::FromString("ERR"));
        
        // Timer to reset the display after showing error
        FTimerHandle UnusedHandle;
        GetWorldTimerManager().SetTimer(UnusedHandle, [this]()
        {
            CurrentCode = "";
            ScreenText->SetTextRenderColor(FColor::White);
            UpdateDisplay();
        }, 1.5f, false);
	}
}