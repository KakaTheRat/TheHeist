#include "PuzzleComponent.h"

UPuzzleComponent::UPuzzleComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UPuzzleComponent::TrySolveEnigma(const FString& PlayerSolution)
{
	if (IsCompleted == true or EnigmaSolution == PlayerSolution)
	{
		IsCompleted = true;
		return true;
	}
	return false;
}

bool UPuzzleComponent::NeedToCheckSolution(const FString& PlayerSolution) const
{
	if (PlayerSolution.Len() == EnigmaSolution.Len())
	{
		return true;
	}
	return false;
}

