// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnigmaComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THEHEIST_API UEnigmaComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true") )
	FString EnigmaSolution = "0";

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true") )
	bool IsCompleted;

public:
	// Sets default values for this component's properties
	UEnigmaComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	bool TrySolveEnigma(FString PlayerSolution);
	bool NeedToCheckSolution(FString PlayerSolution);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
