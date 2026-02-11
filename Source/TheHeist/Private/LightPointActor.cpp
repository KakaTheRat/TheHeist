// Fill out your copyright notice in the Description page of Project Settings.


#include "LightPointActor.h"

// Sets default values
ALightPointActor::ALightPointActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALightPointActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALightPointActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALightPointActor::SetLightIntensity(float Intensity)
{
	if (PointLight)
		PointLight->SetIntensity(Intensity);
}

void ALightPointActor::SetActive(bool bActive)
{
	if (PointLight)
		PointLight->SetVisibility(bActive);
}

