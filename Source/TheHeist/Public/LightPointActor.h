// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "LightPointActor.generated.h"

UCLASS()
class THEHEIST_API ALightPointActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALightPointActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void SetLightIntensity(float Intensity);
	void SetActive(bool bActive);
	
private:
	UPROPERTY(EditAnywhere)
	USpotLightComponent* SpotLight;

	UPROPERTY(VisibleAnywhere, Category = "Root")
	USceneComponent* SceneRoot;

};
