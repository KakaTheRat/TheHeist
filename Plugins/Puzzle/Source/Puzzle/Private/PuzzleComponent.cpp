#include "PuzzleComponent.h"

#pragma region Initialization

UPuzzleComponent::UPuzzleComponent() : bIsCompleted(false)
{
	PrimaryComponentTick.bCanEverTick = false;
}

#pragma endregion

#pragma region Configuration

/*
 * Sets the puzzle solution
 * 
 * @param NewSolution The new solution string
 */
void UPuzzleComponent::SetSolution(const FString& NewSolution)
{
	PuzzleSolution = NewSolution;
	bIsCompleted = false;
}

/*
 * Resets the puzzle state to incomplete
 */
void UPuzzleComponent::Reset()
{
	bIsCompleted = false;
}

#pragma endregion

#pragma region Validation

/*
 * Validates the player's solution
 * 
 * @param PlayerSolution The solution provided by the player
 * @return true if the solution is correct, false otherwise
 */
bool UPuzzleComponent::ValidateSolution(const FString& PlayerSolution)
{
	if (bIsCompleted)
	{
		return true;
	}

	const bool bIsCorrect = PuzzleSolution.Equals(PlayerSolution, ESearchCase::CaseSensitive);
	
	if (bIsCorrect)
	{
		bIsCompleted = true;
	}

	NotifyObservers(bIsCorrect);
	
	return bIsCorrect;
}

/*
 * Checks if the solution length is correct
 * 
 * @param PlayerSolution The solution provided by the player
 * @return true if the lengths match, false otherwise
 */
bool UPuzzleComponent::IsSolutionLengthCorrect(const FString& PlayerSolution) const
{
	return PlayerSolution.Len() == PuzzleSolution.Len();
}

#pragma endregion

#pragma region Observer

/*
 * Adds an observer to be notified of puzzle events
 * 
 * @param Observer The observer to add
 */
void UPuzzleComponent::AddObserver(const TScriptInterface<IPuzzleObserver>& Observer)
{
	if (Observer.GetInterface())
	{
		Observers.AddUnique(Observer);
	}
}

/*
 * Removes an observer
 * 
 * @param Observer The observer to remove
 */
void UPuzzleComponent::RemoveObserver(const TScriptInterface<IPuzzleObserver>& Observer)
{
	Observers.Remove(Observer);
}

/*
 * Notifies all registered observers of puzzle events
 * 
 * @param bSuccess true if the puzzle was solved, false if failed
 */
void UPuzzleComponent::NotifyObservers(bool bSuccess)
{
	for (const TScriptInterface<IPuzzleObserver>& Observer : Observers)
	{
		if (Observer.GetInterface())
		{
			if (bSuccess)
			{
				Observer->OnPuzzleSolved();
			}
			else
			{
				Observer->OnPuzzleFailed();
			}
		}
	}
}

#pragma endregion