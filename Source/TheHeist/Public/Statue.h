// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PointLightComponent.h"
#include "LightPointActor.h"
#include "Statue.generated.h"

UCLASS()
class THEHEIST_API AStatue : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStatue();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	bool VerrifyAngle();
	
	bool GetIsHitStatue() {return bIsHitStatue;}
	
	bool GetIsGreateAngle() {return bIsGreateAngle;}
	
	bool GetIsAvailableStatue() {return bIsAvailableStatue;}
	
private: 
	void Raycast();
	void RaycastAfterRotation();
	void SpawnLightsBetween(const FVector& Start, const FVector& End, bool bStrong);
 
	void UpdateRotation();
	
	UFUNCTION(BlueprintCallable)
	void RotateRight();

	ALightPointActor* FindClosestLight(const FVector& Position,const TArray<AActor*>& Lights);
	FRotator InitalRotator;
	UPROPERTY(EditAnywhere)
	TArray<float> Angles;
	
	FRotator Rotation;
	FRotator CurrentBaseRotation;
	FRotator TargetRotation;
	bool bIsRotating = false;
	
	FRotator StartRotation;
	
	UPROPERTY(EditAnywhere, Category="Rotation")
	float RotationDuration = 0.5f;

	FTimerHandle RotationTimerHandle;
	
	float ElapsedTime = 0.f;
	
	
	bool bIsHitStatue;

	UPROPERTY(EditAnywhere)
	bool bIsAvailableStatue;
	
	bool bIsGreateAngle;
	
	UPROPERTY()
	TArray<ALightPointActor*> CurrentLineLights;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<ALightPointActor> LightSub;

	UPROPERTY(EditAnywhere)
	float LightSpacing = 150.f;

	UPROPERTY(EditAnywhere)
	float StrongIntensity = 8000.f;

	UPROPERTY(EditAnywhere)
	float WeakIntensity = 1000.f;

	UPROPERTY(EditAnywhere)
	float LightRadius = 300.f;
	
	FHitResult LastRaycastHit;

	AStatue* HitStatue;

	FTimerHandle RaycastTimerHandle;	
};
