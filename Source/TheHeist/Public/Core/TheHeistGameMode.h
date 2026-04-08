// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Statue.h"
#include "TheHeistGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllActivatedStatue);
/**
 *  Simple GameMode for a first person game
 */
UCLASS(abstract)
class ATheHeistGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATheHeistGameMode();
	
	void virtual BeginPlay() override;
	UPROPERTY(BlueprintAssignable)
	FOnAllActivatedStatue OnAllActivatedStatue;
	private:
	UFUNCTION()
	void VerrifyStatue();
	UPROPERTY()
	TArray<AStatue*> Statues;
	
	bool bIsGoodStatue = true;
};



