// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectBase.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SplineComponent.h"
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
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USplineComponent* TrajectorySpline;

	
	virtual void UseObject_Implementation() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Throw Settings")
	float MinForce = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Throw Settings")
	float MaxForce = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Throw Settings")
	float ChargeTimeToMax = 1.5f;

	float CurrentForce = 0.f;
	bool bIsCharging = false;
	float ChargeStartTime = 0.f;

	FTimerHandle PreviewTimerHandle;
	bool bIsPreviewing = false;

	

	float LaunchSpeed;

public:
	UFUNCTION(BlueprintCallable)
	void StartCharging();
	UFUNCTION(BlueprintCallable)
	void StopCharging();

	UFUNCTION(BlueprintCallable)
	void StartPreview();
	UFUNCTION(BlueprintCallable)
	void StopPreview();


protected:
	void UpdatePreview();
	void PreviewTrajectory(float Force);

	void ApplyThrow(float Force);
	void ClearSpline();
	
};
