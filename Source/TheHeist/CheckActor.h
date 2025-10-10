// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "CheckActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFindPlayer);

UCLASS()
class THEHEIST_API ACheckActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckActor();
	UPROPERTY(BlueprintAssignable)
	FOnFindPlayer OnFindPlayer;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	void DoRaycast();

	UPROPERTY(EditAnywhere, Category = "Raycast")
	float RayLength = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Raycast")
	float ConeAngle = 30.f; // En degrés

	UPROPERTY(EditAnywhere, Category = "Raycast")
	int32 NumRays = 10;

	UPROPERTY(EditAnywhere, Category = "Raycast")
	FVector RayOffset = FVector(0.f, 0.f, 0.f);

	class ACharacter* PlayerCharacter;

	bool bHasDetectedPlayer = false;
};
