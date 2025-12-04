#include "SecurityCamera.h"

#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"

ASecurityCamera::ASecurityCamera(): InitialRotation()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASecurityCamera::BeginPlay()
{
    Super::BeginPlay();
    
    InitialRotation = GetActorRotation();
    CurrentRotation = 0.0f;

    OnHackCamera.AddDynamic(this, &ASecurityCamera::HackCamera);
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

    //TODO : A supprimer lorsque le hack sera implémenté correctement
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC && PC->WasInputKeyJustPressed(EKeys::SpaceBar))
    {
        OnHackCamera.Broadcast();
    }
}

void ASecurityCamera::HackCamera()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC)
    {
        return;
    }

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    if (!bCameraHacked)
    {
        PC->SetViewTarget(this);
        bCameraHacked = true;

        if (PlayerPawn)
        {
            PlayerPawn->DisableInput(PC);
        }
    }
    else
    {
        if (PlayerPawn)
        {
            PC->SetViewTarget(PlayerPawn);
            PlayerPawn->EnableInput(PC);
        }

        bCameraHacked = false;
    }
}
