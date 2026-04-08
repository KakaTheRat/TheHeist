// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Statue.h"
#include "TheHeistGameMode.generated.h"

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
	
	private:
	UFUNCTION()
	void VerrifyStatue();
	UPROPERTY()
	TArray<AStatue*> Statues;
	
	bool bIsGoodStatue = true;
};



