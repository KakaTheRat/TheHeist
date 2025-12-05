// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Actors/SittingAction.h"


// Sets default values
ASittingAction::ASittingAction()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASittingAction::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASittingAction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

