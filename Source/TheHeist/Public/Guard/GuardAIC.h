// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TheHeistCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GuardAIC.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMaxLevelStress);

UCLASS()
class THEHEIST_API AGuardAIC : public AAIController
{
	GENERATED_BODY()

public:

	//----------Properties----------//

	//Event property, called whenever the guard reaches max stress level
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnMaxLevelStress OnMaxLevelStress;

	protected:

	void BeginPlay() override;
	
};
