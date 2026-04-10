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

	//Returns the player controller
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent, Category="Entities")
	APlayerController* GetPlayerController();
	
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent, Category="Entities")
	void MoveAndLookEntity(USceneComponent* Position, USceneComponent* LookTarget);

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent, Category="Entities")
	void CheckClosest(USceneComponent* InPosition, USceneComponent* OutPosition);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Entities")
	void LookAtTarget(USceneComponent* Target, float BlendTime);
	
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent, Category="Entities")
	void SetIKTarget(FName BoneName, FTransform IKTarget, bool bEnable);

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent, Category="Entities")
	void SetAnimationState(bool bShouldActivate);

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent, Category="Entities")
	void FootStepTriggered(bool bShouldActivate);
};
