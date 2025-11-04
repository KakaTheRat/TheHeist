// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Datas/DataAssets/FormationData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FormationHelper.generated.h"

/**
 * 
 */
UCLASS()
class THEHEIST_API UFormationHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	//Calculates the offset position for each element in an array
	UFUNCTION(BlueprintCallable, Category = "Formation")
	static FVector GetFormationOffset(UFormationData* FormationType,int32 Index, float Spacing = 200.f, float RowSpacing = 200.f);
	
	
};
