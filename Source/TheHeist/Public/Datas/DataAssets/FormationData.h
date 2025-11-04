// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Datas/Enumerators/Guard/FormationType.h"
#include "Engine/DataAsset.h"
#include "FormationData.generated.h"

/**
 * 
 */
UCLASS()
class THEHEIST_API UFormationData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EFormationType FormationType;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FVector Offset;
	
};
