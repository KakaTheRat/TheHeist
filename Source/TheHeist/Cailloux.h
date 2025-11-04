// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectBase.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Cailloux.generated.h"

/**
 * 
 */
UCLASS()
class THEHEIST_API ACailloux : public AObjectBase
{
	GENERATED_BODY()
	ACailloux();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovementComponent;
	
	
};
