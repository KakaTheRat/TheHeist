#include "Pawn/SecurityCamera/SurveillanceRoomActor.h"
#include "Core/TheHeistPlayerController.h"
#include "GameFramework/Character.h"

#pragma region Initialize

ASurveillanceRoomActor::ASurveillanceRoomActor()
{
    PrimaryActorTick.bCanEverTick = false;

    RoomTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("RoomTrigger"));
    RootComponent = RoomTrigger;
    RoomTrigger->SetBoxExtent(FVector(500.f, 500.f, 200.f));
    RoomTrigger->SetCollisionProfileName(TEXT("Trigger"));
}

void ASurveillanceRoomActor::BeginPlay()
{
    Super::BeginPlay();

    RoomTrigger->OnComponentBeginOverlap.AddDynamic(this, &ASurveillanceRoomActor::OnPlayerEnter);
    RoomTrigger->OnComponentEndOverlap.AddDynamic(this, &ASurveillanceRoomActor::OnPlayerExit);

    for (AMonitorActor* Monitor : Monitors)
    {
        if (!Monitor)
        {
            continue;
        }
        Monitor->OnMonitorFocused.AddDynamic(this, &ASurveillanceRoomActor::OnMonitorFocused);
        Monitor->OnMonitorUnfocused.AddDynamic(this, &ASurveillanceRoomActor::OnMonitorUnfocused);
    }
}

#pragma endregion

#pragma region Trigger

void ASurveillanceRoomActor::OnPlayerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (bPlayerInside || !Cast<ACharacter>(OtherActor))
    {
        return;
    }

    bPlayerInside = true;

    for (AMonitorActor* Monitor : Monitors)
    {
        if (Monitor)
        {
            Monitor->ActivateCurrentCamera();
        }
    }
}

void ASurveillanceRoomActor::OnPlayerExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!bPlayerInside || !Cast<ACharacter>(OtherActor))
    {
        return;
    }

    bPlayerInside = false;

    if (FocusedMonitor)
    {
        UnfocusCurrentMonitor();
    }

    for (AMonitorActor* Monitor : Monitors)
    {
        if (Monitor)
        {
            Monitor->DeactivateCurrentCamera();
        }
    }
}

#pragma endregion

#pragma region Focus

void ASurveillanceRoomActor::FocusMonitor(AMonitorActor* Monitor)
{
    if (!Monitor)
    {
        return;
    }

    if (FocusedMonitor && FocusedMonitor != Monitor)
    {
        FocusedMonitor->SetFocused(false);
    }

    FocusedMonitor = Monitor;
    FocusedMonitor->SetFocused(true);
}

void ASurveillanceRoomActor::UnfocusCurrentMonitor()
{
    if (!FocusedMonitor)
    {
        return;
    }

    FocusedMonitor->SetFocused(false);
    FocusedMonitor = nullptr;
}

void ASurveillanceRoomActor::OnMonitorFocused(AMonitorActor* Monitor)
{
    if (ATheHeistPlayerController* PC = Cast<ATheHeistPlayerController>(GetWorld()->GetFirstPlayerController()))
        PC->EnableMonitorInput();
}

void ASurveillanceRoomActor::OnMonitorUnfocused(AMonitorActor* Monitor)
{
    if (ATheHeistPlayerController* PC = Cast<ATheHeistPlayerController>(GetWorld()->GetFirstPlayerController()))
    {
        PC->DisableMonitorInput();
    }
}

#pragma endregion