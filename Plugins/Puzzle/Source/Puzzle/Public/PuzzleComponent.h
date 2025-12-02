#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "PuzzleComponent.generated.h"

UCLASS()
class PUZZLE_API UPuzzleComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true") )
	FString EnigmaSolution = "0";

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true") )
	bool IsCompleted;

public:
	// Sets default values for this component's properties
	UPuzzleComponent();

	bool TrySolveEnigma(const FString& PlayerSolution);
	bool NeedToCheckSolution(const FString& PlayerSolution) const;
};
