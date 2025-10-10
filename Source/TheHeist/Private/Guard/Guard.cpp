#include "Guard/Guard.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"

// Sets default values
AGuard::AGuard()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGuard::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AGuard::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Renvoie la localisation du garde
FVector AGuard::GetGuardLocation() const
{
	return GetActorLocation();
}
