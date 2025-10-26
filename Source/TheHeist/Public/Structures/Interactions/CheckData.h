// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structures/Interactions/InteractionData.h"
#include "CheckData.generated.h"

/**
 * 
 */
UCLASS()
class THEHEIST_API UCheckData : public UInteractionData
{
	GENERATED_BODY()
public:
	UCheckData();

	//Where to check
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Check", meta = (GetOptions = "f"))
	FName LookTargetName;
	
	//Where to check
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Check")
	USceneComponent* LookTarget = nullptr;

	//Checking duration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Check")
	float CheckDuration = 2.0f;

protected:
	virtual void ExecuteInteraction(AActor* Owner, USceneComponent* Target) override;
	
	FTransform OriginalCameraTransform;

private:
	bool bIsChecking = false;
	FTimerHandle CheckTimerHandle;
};
