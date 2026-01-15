#include "Entities/Guard/GuardAIC.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Widget/DetectionMeter/DetectionMeterWidget.h"

#pragma region Initialize

AGuardAIC::AGuardAIC()
{
    PrimaryActorTick.bCanEverTick = true;

    MaxDetectionLevel = 100;
    DetectionTickDelay = 0.05f;
    BlinkDuration = 1.5f;

    SightRadius = 2000.f;
    LoseSightRadius = 2500.f;
    PeripheralVisionAngleDegrees = 90.f;

    CurrentDetectionLevel = 0;
    bPlayerVisible = false;

    AIPerceptionComponent = nullptr;
    SightConfig = nullptr;

    DetectionWidget = nullptr;
    TrackedPlayer = nullptr;
}

void AGuardAIC::BeginPlay()
{
    Super::BeginPlay();
    SetupPerception();
}

void AGuardAIC::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (DetectionWidget)
    {
        UpdateWidgetAngle();
    }
}

#pragma endregion

/*
 * Setup AI Perception Component with Sight Sense
 */
void AGuardAIC::SetupPerception()
{
    AIPerceptionComponent = GetPerceptionComponent();
    if (!AIPerceptionComponent)
        return;

    SightConfig = AIPerceptionComponent->GetSenseConfig<UAISenseConfig_Sight>();
    if (!SightConfig)
    {
        SightConfig = NewObject<UAISenseConfig_Sight>(this);
    }

    SightConfig->SightRadius = SightRadius;
    SightConfig->LoseSightRadius = LoseSightRadius;
    SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngleDegrees;
    SightConfig->SetMaxAge(5.0f);

    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

    AIPerceptionComponent->ConfigureSense(*SightConfig);
    AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

    AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(
        this, &AGuardAIC::OnTargetPerceptionUpdated);
}

/*
 * Perception callback
 */
void AGuardAIC::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor)
    {
        return;
    }

    APawn* DetectedPawn = Cast<APawn>(Actor);
    if (!DetectedPawn || !Cast<APlayerController>(DetectedPawn->GetController()))
    {
        return;
    }

    Stimulus.WasSuccessfullySensed()
        ? OnPlayerDetected(Actor)
        : OnPlayerLost();
}

/*
 * Player detected
 */
void AGuardAIC::OnPlayerDetected(AActor* DetectedPlayer)
{
    TrackedPlayer = DetectedPlayer;
    bPlayerVisible = true;

    if (!DetectionWidget)
    {
        CreateDetectionWidget();
    }

    if (!GetWorldTimerManager().IsTimerActive(DetectionTimerHandle))
    {
        GetWorldTimerManager().SetTimer(
            DetectionTimerHandle,
            this,
            &AGuardAIC::DetectionLoop,
            DetectionTickDelay,
            true);
    }
}

/*
 * Player lost
 */
void AGuardAIC::OnPlayerLost()
{
    bPlayerVisible = false;

    if (!GetWorldTimerManager().IsTimerActive(DetectionTimerHandle))
    {
        GetWorldTimerManager().SetTimer(
            DetectionTimerHandle,
            this,
            &AGuardAIC::DetectionLoop,
            DetectionTickDelay,
            true);
    }
}

/*
 * Detection progression loop
 */
void AGuardAIC::DetectionLoop()
{
    CurrentDetectionLevel += bPlayerVisible ? 1 : -1;
    CurrentDetectionLevel = FMath::Clamp(CurrentDetectionLevel, 0, MaxDetectionLevel);

    if (DetectionWidget)
    {
        DetectionWidget->UpdatePercent(
            static_cast<float>(CurrentDetectionLevel) / MaxDetectionLevel);
    }

    if (CurrentDetectionLevel == MaxDetectionLevel)
    {
        GetWorldTimerManager().ClearTimer(DetectionTimerHandle);
        PlayerFullyDetected();
    }
    else if (CurrentDetectionLevel == 0)
    {
        GetWorldTimerManager().ClearTimer(DetectionTimerHandle);
        PlayerDetectionLost();
    }
}

/*
 * UI creation
 */
void AGuardAIC::CreateDetectionWidget()
{
    if (!DetectionWidgetClass)
    {
        return;
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC)
    {
        return;
    }

    DetectionWidget = CreateWidget<UDetectionMeterWidget>(PC, DetectionWidgetClass);
    if (DetectionWidget)
    {
        DetectionWidget->AddToViewport(100);
        DetectionWidget->UpdatePercent(0.f);
    }
}

/*
 * UI cleanup
 */
void AGuardAIC::RemoveDetectionWidget()
{
    if (DetectionWidget)
    {
        DetectionWidget->RemoveFromParent();
        DetectionWidget = nullptr;
    }
}

/*
 * Widget orientation
 */
void AGuardAIC::UpdateWidgetAngle()
{
    if (!TrackedPlayer)
    {
        return;
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC)
    {
        return;
    }

    FVector GuardLocation = GetPawn()->GetActorLocation();
    FVector PlayerLocation = TrackedPlayer->GetActorLocation();

    float Yaw = FMath::UnwindDegrees(
        (GuardLocation - PlayerLocation).Rotation().Yaw
        - PC->GetControlRotation().Yaw);

    DetectionWidget->UpdateAngle(Yaw);
}

/*
 * Detection max
 */
void AGuardAIC::PlayerFullyDetected()
{
    if (DetectionWidget)
    {
        DetectionWidget->BlinkIcon();
    }

    OnMaxLevelStress.Broadcast();
}

/*
 * Detection lost
 */
void AGuardAIC::PlayerDetectionLost()
{
    RemoveDetectionWidget();
    TrackedPlayer = nullptr;
}
