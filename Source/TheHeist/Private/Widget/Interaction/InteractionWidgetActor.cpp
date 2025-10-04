// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Interaction/InteractionWidgetActor.h"

// Sets default values
AInteractionWidgetActor::AInteractionWidgetActor()
{
	PrimaryActorTick.bCanEverTick = false;

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	RootComponent = WidgetComp;

	WidgetComp->SetWidgetSpace(EWidgetSpace::World);
	WidgetComp->SetDrawSize(FVector2D(300, 100)); // Size

}


// Called when the game starts or when spawned
void AInteractionWidgetActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractionWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

