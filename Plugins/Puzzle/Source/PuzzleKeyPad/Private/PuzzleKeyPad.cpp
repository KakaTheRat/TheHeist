#include "PuzzleKeyPad.h"

#include "Components/WidgetComponent.h"

#pragma region Initialization

APuzzleKeyPad::APuzzleKeyPad()
    : CurrentCode("")
    , ErrorDisplayDuration(1.5f)
{
    PrimaryActorTick.bCanEverTick = false;

    // Create root component
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    // Create logic components
    PuzzleComponent = CreateDefaultSubobject<UPuzzleComponent>(TEXT("PuzzleComponent"));
    DisplayManager = CreateDefaultSubobject<UPuzzleDisplayManager>(TEXT("DisplayManager"));

    // Create visual component
    ScreenWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ScreenWidget"));
    ScreenWidget->SetupAttachment(RootComponent);
}

void APuzzleKeyPad::BeginPlay()
{
    Super::BeginPlay();
    
    InitializeComponents();
}

void APuzzleKeyPad::InitializeComponents()
{
    if (DisplayManager)
    {
       DisplayManager->Initialize(ScreenWidget);
       DisplayManager->UpdateDisplay("");
    }

    if (PuzzleComponent)
    {
       PuzzleComponent->AddObserver(this);
    }
}

#pragma endregion

#pragma region Public Interface

/**
 * Receives input from external sources (e.g., player interaction)
 * 
 * @param Value The input value from the keypad
 */
void APuzzleKeyPad::ReceiveInput(FString Value)
{
    if (!PuzzleComponent)
    {
    	UE_LOG(LogTemp, Warning, TEXT("PuzzleComponent is not initialized."));
       return;
    }

    AppendToCode(Value);

    UpdateDisplay();

    if (IsCodeComplete())
    {
       RequestValidation();
    }
}

/**
 * Clears the current code
 */
void APuzzleKeyPad::ClearCode()
{
    CurrentCode.Empty();
    UpdateDisplay();
}

/**
 * Sets the puzzle solution
 * 
 * @param Solution The correct solution string
 */
void APuzzleKeyPad::SetPuzzleSolution(const FString& Solution)
{
    if (PuzzleComponent)
    {
       PuzzleComponent->SetSolution(Solution);
    }
}

#pragma endregion

#pragma region Code Management

/**
 * Appends a value to the current code
 * 
 * @param Value The value to append
 */
void APuzzleKeyPad::AppendToCode(const FString& Value)
{
    CurrentCode.Append(Value);
}

/**
 * Checks if the code is complete (reached expected length)
 * 
 * @return true if the code length matches the solution length
 */
bool APuzzleKeyPad::IsCodeComplete() const
{
    if (!PuzzleComponent)
    {
       return false;
    }

    return CurrentCode.Len() == PuzzleComponent->GetSolutionLength();
}

/**
 * Updates the display with the current code
 */
void APuzzleKeyPad::UpdateDisplay()
{
    if (DisplayManager)
    {
       DisplayManager->UpdateDisplay(CurrentCode);
    }
}

/**
 * Requests validation of the current code
 */
void APuzzleKeyPad::RequestValidation()
{
    if (PuzzleComponent)
    {
       PuzzleComponent->ValidateSolution(CurrentCode);
    }
}

#pragma endregion

#pragma region Observer Implementation

/**
 * Called when the puzzle is successfully solved
 */
void APuzzleKeyPad::OnPuzzleSolved()
{
    if (DisplayManager)
    {
       DisplayManager->ShowValidationResult(true);
    }

    OnPuzzleSolvedEvent();
}

/**
 * Called when puzzle validation fails
 */
void APuzzleKeyPad::OnPuzzleFailed()
{
    if (DisplayManager)
    {
       DisplayManager->ShowValidationResult(false);
    }

    GetWorldTimerManager().SetTimer(
       ErrorResetTimerHandle,
       this,
       &APuzzleKeyPad::ResetAfterError,
       ErrorDisplayDuration,
       false
    );

    OnPuzzleFailedEvent();
}

#pragma endregion

#pragma region Reset Logic

/**
 * Resets the display and input after showing an error
 */
void APuzzleKeyPad::ResetAfterError()
{
    ClearCode();

    if (DisplayManager)
    {
       DisplayManager->ResetDisplay();
    }
}

#pragma endregion