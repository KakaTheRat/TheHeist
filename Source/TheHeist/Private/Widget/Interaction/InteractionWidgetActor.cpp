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
	WidgetComp->SetPivot(FVector2D(0.5f, 0.5f));

}

void AInteractionWidgetActor::ShowWidget(const FVector& Location)
{
	SetActorLocation(Location);
	if (MyWidget)
		SetActorHiddenInGame(false);
}

void AInteractionWidgetActor::HideWidget()
{
	SetActorHiddenInGame(true);
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

