#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "PuzzleComponent.generated.h"

#pragma region PuzzleObserver Interface

/**
 * Interface for puzzle state observers
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UPuzzleObserver : public UInterface
{
	GENERATED_BODY()
};

class IPuzzleObserver
{
	GENERATED_BODY()

public:
	
	/* Called when the puzzle is successfully solved */
	virtual void OnPuzzleSolved() = 0;

	/* Called when the puzzle validation fails */
	virtual void OnPuzzleFailed() = 0;
};

#pragma endregion

/**
 * Component managing puzzle solving logic
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PUZZLE_API UPuzzleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UPuzzleComponent();

	/* Sets the puzzle solution */
	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	void SetSolution(const FString& NewSolution);

	/* Validates the player's solution */
	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	bool ValidateSolution(const FString& PlayerSolution);

	/* Checks if the solution length is correct */
	UFUNCTION(BlueprintPure, Category = "Puzzle")
	bool IsSolutionLengthCorrect(const FString& PlayerSolution) const;

	/* Returns the puzzle completion state */
	UFUNCTION(BlueprintPure, Category = "Puzzle")
	bool IsCompleted() const { return bIsCompleted; }

	/* Gets the expected solution length */
	UFUNCTION(BlueprintPure, Category = "Puzzle")
	int32 GetSolutionLength() const { return PuzzleSolution.Len(); }

	/* Resets the puzzle to its initial state */
	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	void Reset();

	/* Adds an observer to be notified of puzzle events */
	void AddObserver(const TScriptInterface<IPuzzleObserver>& Observer);

	/* Removes an observer */
	void RemoveObserver(const TScriptInterface<IPuzzleObserver>& Observer);

protected:
	
	/** The correct solution to the puzzle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	FString PuzzleSolution;

	/** Whether the puzzle has been completed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Puzzle")
	bool bIsCompleted;

private:
	
	/* Notifies all registered observers of puzzle events*/
	void NotifyObservers(bool bSuccess);
	
	/** List of registered observers */
	TArray<TScriptInterface<IPuzzleObserver>> Observers;
};