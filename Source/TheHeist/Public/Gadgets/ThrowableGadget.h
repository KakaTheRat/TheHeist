// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gadgets/Gadgets.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ThrowableGadget.generated.h"

/**
 * 
 */
UCLASS()
class THEHEIST_API AThrowableGadget : public AGadgets
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Gadgets")
	UProjectileMovementComponent* ProjectileMovement;
	
};
