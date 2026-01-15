#include "Pawn/SecurityCamera/SecurityCamera.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "Widget/DetectionMeter/DetectionMeterWidget.h"

#pragma region Initialize

ASecurityCamera::ASecurityCamera()
{
    PrimaryActorTick.bCanEverTick = true;

    AIPerceptionComponent = nullptr;
    SightConfig = nullptr;

    CurrentDetectionLevel = 0;
    bPlayerVisible = false;
    bCameraHacked = false;

    DetectionWidget = nullptr;
    TrackedPlayer = nullptr;
}

void ASecurityCamera::BeginPlay()
{
    Super::BeginPlay();

    InitialRotation = GetActorRotation();
    SetupPerception();
}

void ASecurityCamera::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsMoving && !bCameraHacked)
    {
        const float DeltaRotation = RotationSpeed * DeltaTime;

        CurrentRotation += bRotatingRight ? DeltaRotation : -DeltaRotation;

        if (CurrentRotation >= MaxRotationAngle)
        {
            CurrentRotation = MaxRotationAngle;
            bRotatingRight = false;
        }
        else if (CurrentRotation <= -MaxRotationAngle)
        {
            CurrentRotation = -MaxRotationAngle;
            bRotatingRight = true;
        }

        FRotator NewRotation = InitialRotation;
        NewRotation.Yaw += CurrentRotation;
        SetActorRotation(NewRotation);
    }

    if (DetectionWidget && IsValid(DetectionWidget))
    {
        UpdateWidgetAngle();
    }
}

#pragma endregion

/*
 *  Setup AI Perception Component with Sight Sense
 */
void ASecurityCamera::SetupPerception()
{
    AIPerceptionComponent = FindComponentByClass<UAIPerceptionComponent>();
    if (!AIPerceptionComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("[SecurityCamera] Missing AIPerceptionComponent"));
        return;
    }

    SightConfig = AIPerceptionComponent->GetSenseConfig<UAISenseConfig_Sight>();
    if (!SightConfig)
    {
        SightConfig = NewObject<UAISenseConfig_Sight>(this);
    }

    SightConfig->SightRadius = SightRadius;
    SightConfig->LoseSightRadius = LoseSightRadius;
    SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngleDegrees;
    SightConfig->SetMaxAge(5.0f);
    SightConfig->AutoSuccessRangeFromLastSeenLocation = 500.0f;

    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

    AIPerceptionComponent->ConfigureSense(*SightConfig);
    AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

    if (!AIPerceptionComponent->OnTargetPerceptionUpdated.IsAlreadyBound(
            this, &ASecurityCamera::OnTargetPerceptionUpdated))
    {
        AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(
            this, &ASecurityCamera::OnTargetPerceptionUpdated);
    }

    AIPerceptionComponent->RequestStimuliListenerUpdate();
}

/*
 * Callback call when perception of a target is updated
 */
void ASecurityCamera::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor || bCameraHacked)
    {
        return;
    }

    APawn* DetectedPawn = Cast<APawn>(Actor);
    if (!DetectedPawn)
    {
        return;
    }

    if (!Cast<APlayerController>(DetectedPawn->GetController()))
    {
        return;
    }

    Stimulus.WasSuccessfullySensed()
        ? OnPlayerDetected(Actor)
        : OnPlayerLost();
}

/*
 * Call when the player is detected
 */
void ASecurityCamera::OnPlayerDetected(AActor* DetectedPlayer)
{
    if (!DetectedPlayer || bCameraHacked)
    {
        return;
    }

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
            &ASecurityCamera::DetectionLoop,
            DetectionTickDelay,
            true);
    }
}

/*
 * Call when the player is lost
 */
void ASecurityCamera::OnPlayerLost()
{
    bPlayerVisible = false;

    if (!GetWorldTimerManager().IsTimerActive(DetectionTimerHandle))
    {
        GetWorldTimerManager().SetTimer(
            DetectionTimerHandle,
            this,
            &ASecurityCamera::DetectionLoop,
            DetectionTickDelay,
            true);
    }
}

/*
 * Loop for updating detection level
 */
void ASecurityCamera::DetectionLoop()
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
        
        if (DetectionWidget)
        {
            DetectionWidget->BlinkIcon();
        }
        
        OnPlayerDetectedEvent.Broadcast();
    }
    else if (CurrentDetectionLevel == 0)
    {
        GetWorldTimerManager().ClearTimer(DetectionTimerHandle);
        PlayerDetectionLost();
    }
}

/*
 * Create widget detection
 */
void ASecurityCamera::CreateDetectionWidget()
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
        DetectionWidget->SetTrackedPlayer(TrackedPlayer);
        
        DetectionWidget->OnBlinkAnimationFinished.AddDynamic(this, &ASecurityCamera::OnBlinkCompleted);
    }
}

/*
 * Remove widget detection
 */
void ASecurityCamera::RemoveDetectionWidget()
{
    if (DetectionWidget)
    {
        DetectionWidget->ResetDetection();
        DetectionWidget->RemoveFromParent();
        DetectionWidget = nullptr;
    }
}

/*
 * Update widget angle to point towards the player
 */
void ASecurityCamera::UpdateWidgetAngle()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC || !TrackedPlayer)
    {
        return;
    }

    FVector CameraLocation = GetActorLocation();
    FVector PlayerLocation = TrackedPlayer->GetActorLocation();

    float Yaw = FMath::UnwindDegrees(
        (CameraLocation - PlayerLocation).Rotation().Yaw
        - PC->GetControlRotation().Yaw);

    DetectionWidget->UpdateAngle(Yaw);
}

/*
 * When the player is fully detected
 */
void ASecurityCamera::PlayerFullyDetected()
{
    //TODO : ajouter des effets sonores ou visuels ici et logique de gameplay
}

/*
 * Callback when blink animation is completed
 */
void ASecurityCamera::OnBlinkCompleted()
{
    RemoveDetectionWidget();
}

/*
 * When the player detection is lost
 */
void ASecurityCamera::PlayerDetectionLost()
{
    RemoveDetectionWidget();
    TrackedPlayer = nullptr;
}

/*
 * Hack the security camera
 */
void ASecurityCamera::HackCamera()
{
    bCameraHacked = true;

    GetWorldTimerManager().ClearTimer(DetectionTimerHandle);
    RemoveDetectionWidget();

    CurrentDetectionLevel = 0;
    bPlayerVisible = false;
    TrackedPlayer = nullptr;

    if (AIPerceptionComponent)
    {
        AIPerceptionComponent->SetSenseEnabled(
            UAISense_Sight::StaticClass(), false);
    }

    OnHackCamera.Broadcast();
}