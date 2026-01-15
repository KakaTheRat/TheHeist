#include "Entities/Guard/GuardAIC.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Widget/DetectionMeter/DetectionMeterWidget.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

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

    OutlineFadeDuration = 2.0f;
    SuspicionThreshold = 0.3f;
    AlertThreshold = 0.7f;

    CurrentDetectionLevel = 0;
    bPlayerVisible = false;

    AIPerceptionComponent = nullptr;
    SightConfig = nullptr;

    DetectionWidget = nullptr;
    TrackedPlayer = nullptr;
    
    OutlineMaterialBase = nullptr;
    OutlineMaterialInstance = nullptr;
}

void AGuardAIC::BeginPlay()
{
    Super::BeginPlay();
    SetupPerception();
    
    if (OutlineMaterialBase)
    {
        OutlineMaterialInstance = UMaterialInstanceDynamic::Create(OutlineMaterialBase, this);
    }
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

    if (GetWorldTimerManager().IsTimerActive(OutlineFadeTimerHandle))
    {
        GetWorldTimerManager().ClearTimer(OutlineFadeTimerHandle);
    }

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

    if (OutlineFadeDuration > 0.f && TrackedPlayer)
    {
        GetWorldTimerManager().SetTimer(
            OutlineFadeTimerHandle,
            this,
            &AGuardAIC::RemoveGuardOutline,
            OutlineFadeDuration,
            false);
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
 * Detection progression loop
 */
void AGuardAIC::DetectionLoop()
{
    CurrentDetectionLevel += bPlayerVisible ? 1 : -1;
    CurrentDetectionLevel = FMath::Clamp(CurrentDetectionLevel, 0, MaxDetectionLevel);

    UpdateGuardOutline();

    if (DetectionWidget)
    {
        DetectionWidget->UpdatePercent(
            static_cast<float>(CurrentDetectionLevel) / MaxDetectionLevel);
    }

    if (CurrentDetectionLevel == MaxDetectionLevel)
    {
        GetWorldTimerManager().ClearTimer(DetectionTimerHandle);
        //PlayerFullyDetected(TODO);
    }
    else if (CurrentDetectionLevel == 0)
    {
        GetWorldTimerManager().ClearTimer(DetectionTimerHandle);
        PlayerDetectionLost();
    }
}

/*
 * Update guard outline based on detection level
 */
void AGuardAIC::UpdateGuardOutline()
{
    if (!TrackedPlayer)
    {
        return;
    }

    float DetectionPercent = static_cast<float>(CurrentDetectionLevel) / MaxDetectionLevel;
    
    FLinearColor OutlineColor;
    float OutlineWidth = 1.0f;

    if (DetectionPercent >= AlertThreshold)
    {
        OutlineColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
    }
    else if (DetectionPercent >= SuspicionThreshold)
    {
        float LerpAlpha = (DetectionPercent - SuspicionThreshold) / (AlertThreshold - SuspicionThreshold);
        OutlineColor = FLinearColor::LerpUsingHSV(
            FLinearColor(1.0f, 0.6f, 0.0f, 1.0f),
            FLinearColor(1.0f, 0.0f, 0.0f, 1.0f),
            LerpAlpha
        );
    }
    else if (DetectionPercent > 0.0f)
    {
        OutlineColor = FLinearColor(1.0f, 1.0f, 0.0f, 1.0f);
    }
    else
    {
        RemoveGuardOutline();
        return;
    }

    ApplyOutlineToGuard(OutlineColor, OutlineWidth);
}

/*
 * Apply outline effect to GUARD using Overlay Material
 */
void AGuardAIC::ApplyOutlineToGuard(FLinearColor OutlineColor, float OutlineWidth)
{
    APawn* GuardPawn = GetPawn();
    if (!GuardPawn)
    {
        return;
    }

    USkeletalMeshComponent* GuardMesh = GuardPawn->FindComponentByClass<USkeletalMeshComponent>();
    if (!GuardMesh)
    {
        return;
    }

    if (!OutlineMaterialInstance && OutlineMaterialBase)
    {
        OutlineMaterialInstance = UMaterialInstanceDynamic::Create(OutlineMaterialBase, this);
    }
    
    if (OutlineMaterialInstance)
    {
        OutlineMaterialInstance->SetVectorParameterValue(FName("OutlineColor"), OutlineColor);
        OutlineMaterialInstance->SetScalarParameterValue(FName("OutlineWidth"), OutlineWidth);
        
        GuardMesh->SetOverlayMaterial(OutlineMaterialInstance);
    }
}

/*
 * Remove outline effect from GUARD
 */
void AGuardAIC::RemoveGuardOutline()
{
    APawn* GuardPawn = GetPawn();
    if (!GuardPawn)
        return;

    USkeletalMeshComponent* GuardMesh = GuardPawn->FindComponentByClass<USkeletalMeshComponent>();
    if (!GuardMesh)
        return;

    GuardMesh->SetOverlayMaterial(nullptr);

    if (GetWorldTimerManager().IsTimerActive(OutlineFadeTimerHandle))
    {
        GetWorldTimerManager().ClearTimer(OutlineFadeTimerHandle);
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
        DetectionWidget->SetTrackedPlayer(TrackedPlayer);
    }
    DetectionWidget->OnHalfDetectionReached.AddDynamic(this, &AGuardAIC::HandleHalfDetection);
    DetectionWidget->OnMaxDetectionReached.AddDynamic(this, &AGuardAIC::PlayerFullyDetected);
}

/*
 * UI cleanup
 */
void AGuardAIC::RemoveDetectionWidget()
{
    if (DetectionWidget)
    {
        DetectionWidget->ResetDetection();
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
void AGuardAIC::PlayerFullyDetected(AActor* Target)
{
    if (DetectionWidget)
    {
        //DetectionWidget->BlinkIcon();
        RemoveDetectionWidget();
    }

    OnMaxLevelStress.Broadcast();
}

/*
 * Detection lost
 */
void AGuardAIC::PlayerDetectionLost()
{
    RemoveDetectionWidget();
    
    if (OutlineFadeDuration <= 0.f)
    {
        RemoveGuardOutline();
    }
    
    TrackedPlayer = nullptr;
}