// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerInteractionComponent.h"

// Sets default values for this component's properties
UPlayerInteractionComponent::UPlayerInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayerInteractionComponent::BeginPlay()
{
	Super::BeginPlay();


	//Instantiate the interaction choice widget actor
	FActorSpawnParameters Params;
	Params.Owner = GetOwner();
	InteractionChoiceWidgetInstance = GetWorld()->SpawnActor<AInteractionWidgetActor>(
	InteractionChoiceWidgetClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		Params);

	//Instantiate the interaction signal widget actor
	Params.Owner = GetOwner();
	InteractionSignalWidgetInstance = GetWorld()->SpawnActor<AActor>(
	InteractionSignalWidgetClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		Params);
}


// Called every frame
void UPlayerInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


