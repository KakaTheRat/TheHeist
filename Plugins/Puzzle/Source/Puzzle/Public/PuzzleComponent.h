#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "PuzzleComponent.generated.h"

/**
 * Puzzle component to manage enigma solving logic
 */
UCLASS()
class PUZZLE_API UPuzzleComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	/** The correct solution to the puzzle */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true") )
	FString PuzzleSolution = "0";

	/** Whether the puzzle has been completed */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true") )
	bool IsCompleted;

public:
	UPuzzleComponent();

	/** Attempts to solve the puzzle with the provided player solution */
	bool TrySolvePuzzle(const FString& PlayerSolution);

	/** Checks if the player's solution needs to be evaluated */
	bool NeedToCheckSolution(const FString& PlayerSolution) const;
};
