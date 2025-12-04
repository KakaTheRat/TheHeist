// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Actors/ActionActor.h"

// Sets default values
AActionActor::AActionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AActionActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AActionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AActionActor::IsAvailable() const
{
    return RegisteredActors.Num() < MaxUsers;
}

bool AActionActor::RegisterUser(AActor* User)
{
	if (!User || !IsAvailable())
		return false;
    
	RegisteredActors.Add(User);
    
	UE_LOG(LogTemp, Log, TEXT("SmartObject %s: User %s registered (%d/%d)"), 
		*GetName(), *User->GetName(), RegisteredActors.Num(), MaxUsers);
    
	return true;
}

void AActionActor::UnregisterUser(AActor* User)
{
	if (!User)
		return;
    
	RegisteredActors.Remove(User);
    
	UE_LOG(LogTemp, Log, TEXT("SmartObject %s: User %s unregistered (%d/%d)"), 
		*GetName(), *User->GetName(), RegisteredActors.Num(), MaxUsers);
}