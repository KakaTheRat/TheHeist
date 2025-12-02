#include "SecurityCamera.h"
#include "Math/UnrealMathUtility.h"

ASecurityCamera::ASecurityCamera(): InitialRotation()
{
    PrimaryActorTick.bCanEverTick = true;

    AutoPossessPlayer = EAutoReceiveInput::Disabled;
}

void ASecurityCamera::BeginPlay()
{
    Super::BeginPlay();
    
    InitialRotation = GetActorRotation();
    CurrentRotation = 0.0f;
}

void ASecurityCamera::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (bIsMoving)
    {
        float RotationDelta = RotationSpeed * DeltaTime;
        
        if (bRotatingRight)
        {
            CurrentRotation += RotationDelta;
            if (CurrentRotation >= MaxRotationAngle)
            {
                CurrentRotation = MaxRotationAngle;
                bRotatingRight = false;
            }
        }
        else
        {
            CurrentRotation -= RotationDelta;
            if (CurrentRotation <= -MaxRotationAngle)
            {
                CurrentRotation = -MaxRotationAngle;
                bRotatingRight = true;
            }
        }
        
        FRotator NewRotation = InitialRotation;
        NewRotation.Yaw += CurrentRotation;
        SetActorRotation(NewRotation);
    }
}

void ASecurityCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}