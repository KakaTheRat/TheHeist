// Fill out your copyright notice in the Description page of Project Settings.


#include "Gadgets/Gadgets.h"

// Sets default values
AGadgets::AGadgets()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);


}

// Called when the game starts or when spawned
void AGadgets::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGadgets::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGadgets::OnUsePressed_Implementation()
{
}

void AGadgets::OnUseReleased_Implementation()
{
}


