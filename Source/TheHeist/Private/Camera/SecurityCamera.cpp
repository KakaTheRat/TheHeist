#include "Camera/SecurityCamera.h"

// Sets default values
ASecurityCamera::ASecurityCamera()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASecurityCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASecurityCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASecurityCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

