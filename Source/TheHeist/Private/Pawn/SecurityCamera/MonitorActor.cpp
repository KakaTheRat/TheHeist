#include "Pawn/SecurityCamera/MonitorActor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInstanceDynamic.h"

#pragma region Initialize

AMonitorActor::AMonitorActor()
{
    PrimaryActorTick.bCanEverTick = false;

    ScreenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScreenMesh"));
    RootComponent = ScreenMesh;

    InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
}

void AMonitorActor::BeginPlay()
{
    Super::BeginPlay();

    if (!MonitorMaterial || LinkedCameras.Num() == 0)
    {
        return;
    }

    ScreenMesh->SetMaterial(0, MonitorMaterial);
    DynamicMaterial = ScreenMesh->CreateAndSetMaterialInstanceDynamic(0);

    if (!DynamicMaterial)
    {
        return;
    }

    GetWorldTimerManager().SetTimerForNextTick(this, &AMonitorActor::InitScreenTexture);
}

void AMonitorActor::InitScreenTexture()
{
    UpdateScreenTexture();
}

#pragma endregion

#pragma region Texture

void AMonitorActor::UpdateScreenTexture()
{
    if (!DynamicMaterial)
    {
        return;
    }

    ASecurityCamera* Cam = GetCurrentCamera();
    if (!Cam)
    {
        return;
    }

    UTextureRenderTarget2D* RT = Cam->GetRenderTarget();
    if (!RT)
    {
        return;
    }

    DynamicMaterial->SetTextureParameterValue(FName("ScreenTexture"), RT);
}

#pragma endregion

#pragma region Camera

void AMonitorActor::SwitchToCamera(int32 Index)
{
    if (!LinkedCameras.IsValidIndex(Index))
    {
        return;
    }

    if (bRoomActive && LinkedCameras.IsValidIndex(CurrentCameraIndex))
    {
        LinkedCameras[CurrentCameraIndex]->RemoveViewer();
    }

    CurrentCameraIndex = Index;
    UpdateScreenTexture();

    if (bRoomActive && LinkedCameras.IsValidIndex(CurrentCameraIndex))
    {
        LinkedCameras[CurrentCameraIndex]->AddViewer();
    }
}

void AMonitorActor::SwipeNext()
{
    if (LinkedCameras.Num() == 0)
    {
        return;
    }
    SwitchToCamera((CurrentCameraIndex + 1) % LinkedCameras.Num());
}

void AMonitorActor::SwipePrevious()
{
    if (LinkedCameras.Num() == 0)
    {
        return;
    }
    SwitchToCamera((CurrentCameraIndex - 1 + LinkedCameras.Num()) % LinkedCameras.Num());
}

ASecurityCamera* AMonitorActor::GetCurrentCamera() const
{
    return LinkedCameras.IsValidIndex(CurrentCameraIndex) ? LinkedCameras[CurrentCameraIndex] : nullptr;
}

#pragma endregion

#pragma region Capture

void AMonitorActor::ActivateCurrentCamera()
{
    bRoomActive = true;

    if (LinkedCameras.IsValidIndex(CurrentCameraIndex))
    {
        LinkedCameras[CurrentCameraIndex]->AddViewer();
    }
}

void AMonitorActor::DeactivateCurrentCamera()
{
    bRoomActive = false;

    if (LinkedCameras.IsValidIndex(CurrentCameraIndex))
    {
        LinkedCameras[CurrentCameraIndex]->RemoveViewer();
    }

    if (bIsFocused)
    {
        SetFocused(false);
    }
}

#pragma endregion

#pragma region Focus

void AMonitorActor::SetFocused(bool bFocused)
{
    bIsFocused = bFocused;

    if (bFocused)
    {
        OnMonitorFocused.Broadcast(this);
    }
    else
    {
        OnMonitorUnfocused.Broadcast(this);
    }
}

#pragma endregion