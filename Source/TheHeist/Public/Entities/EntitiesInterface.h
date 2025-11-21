// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EntitiesInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UEntitiesInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class THEHEIST_API IEntitiesInterface
{
	GENERATED_BODY()

	public:

	//Returns the animation blueprint
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent, Category="Entities")
	UAnimBlueprint* GetAnimationBlueprint();

	//Returns the skeletal mesh
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent, Category="Entities")
	USkeletalMeshComponent* GetSkeletalMeshComponent();
	
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent, Category="Entities")
	void MoveEntity(USceneComponent* Position);
};
