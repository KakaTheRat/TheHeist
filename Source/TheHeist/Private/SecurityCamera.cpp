// C++
#include "SecurityCamera.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

ASecurityCamera::ASecurityCamera()
{
    PrimaryActorTick.bCanEverTick = false; 
    
    AutoPossessPlayer = EAutoReceiveInput::Disabled;
    bIsActive = true;
}

void ASecurityCamera::BeginPlay()
{
    Super::BeginPlay();

    BaseRotation = InitialRotation + GetActorRotation();
}

void ASecurityCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASecurityCamera::SetActive(bool bEnable)
{
    bIsActive = bEnable;
}
