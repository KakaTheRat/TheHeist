#include "PuzzleComponent.h"

/**
 * Puzzle component to manage enigma solving logic
 */
UPuzzleComponent::UPuzzleComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

/**
 * Attempts to solve the puzzle with the provided player solution
 *
 * @param PlayerSolution The solution provided by the player
 * @return true if the puzzle is solved, false otherwise
 */
bool UPuzzleComponent::TrySolvePuzzle(const FString& PlayerSolution)
{
	if (IsCompleted == true or PuzzleSolution == PlayerSolution)
	{
		IsCompleted = true;
		return true;
	}
	return false;
}

/**
 * Checks if the player's solution needs to be evaluated
 *
 * @param PlayerSolution The solution provided by the player
 * @return true if the player's solution length matches the puzzle solution length, false otherwise
 */
bool UPuzzleComponent::NeedToCheckSolution(const FString& PlayerSolution) const
{
	if (PlayerSolution.Len() == PuzzleSolution.Len())
	{
		return true;
	}
	return false;
}

