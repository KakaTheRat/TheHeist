#include "Entities/Guard/GuardAIC.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Widget/DetectionMeter/DetectionMeterWidget.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Sound/SoundBase.h"

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
    PreviousDetectionLevel = 0;
    bPlayerVisible = false;

    AIPerceptionComponent = nullptr;
    SightConfig = nullptr;

    DetectionWidget = nullptr;
    TrackedPlayer = nullptr;
    
    OutlineMaterialBase = nullptr;
    OutlineMaterialInstance = nullptr;

    bPlayedStartSound = false;
    bPlayedSuspicionSound = false;
    bPlayedAlertSound = false;
    bIsBreathing = false;

    DetectionSoundVolume = 1.0f;
    AmbientSoundVolume = 0.7f;
    FootstepSoundVolume = 0.5f;
    BreathingSoundVolume = 0.6f;
    CustomSoundAttenuation = nullptr;

    bEnableAmbientSounds = true;
    bEnableBreathingSounds = true;
    AmbientSoundMinInterval = 8.0f;
    AmbientSoundMaxInterval = 20.0f;
}

void AGuardAIC::BeginPlay()
{
    Super::BeginPlay();
    SetupPerception();
    
    if (OutlineMaterialBase)
    {
        OutlineMaterialInstance = UMaterialInstanceDynamic::Create(OutlineMaterialBase, this);
    }

    if (bEnableAmbientSounds && AmbientIdleSounds.Num() > 0)
    {
        ScheduleNextAmbientSound();
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

void AGuardAIC::UpdatePlayerHeartbeat()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC || !PC->GetPawn())
    {
        UE_LOG(LogTemp, Warning, TEXT("GuardAIC: No player controller or pawn"));
        return;
    }
    
    float StressLevel = static_cast<float>(CurrentDetectionLevel) / MaxDetectionLevel;
}

#pragma endregion

#pragma region Perception

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

#pragma endregion

#pragma region Detection

void AGuardAIC::DetectionLoop()
{
    PreviousDetectionLevel = CurrentDetectionLevel;
    CurrentDetectionLevel += bPlayerVisible ? 1 : -1;
    CurrentDetectionLevel = FMath::Clamp(CurrentDetectionLevel, 0, MaxDetectionLevel);

    UpdateGuardOutline();

    CheckDetectionThresholds();

    UpdatePlayerHeartbeat();

    if (DetectionWidget)
    {
        DetectionWidget->UpdatePercent(
            static_cast<float>(CurrentDetectionLevel) / MaxDetectionLevel);
    }

    if (CurrentDetectionLevel == MaxDetectionLevel)
    {
        GetWorldTimerManager().ClearTimer(DetectionTimerHandle);
        
        PlayRandomSound(SoundsDetectionFull, DetectionSoundVolume);
        
        if (DetectionWidget)
        {
            DetectionWidget->BlinkIcon();
        }
        
        OnMaxLevelStress.Broadcast();
    }
    else if (CurrentDetectionLevel == 0)
    {
        GetWorldTimerManager().ClearTimer(DetectionTimerHandle);
        PlayerDetectionLost();
    }
}

void AGuardAIC::CheckDetectionThresholds()
{
    float DetectionPercent = static_cast<float>(CurrentDetectionLevel) / MaxDetectionLevel;
    float PreviousPercent = static_cast<float>(PreviousDetectionLevel) / MaxDetectionLevel;

    if (!bPlayedStartSound && DetectionPercent > 0.0f && PreviousPercent == 0.0f)
    {
        bPlayedStartSound = true;
        PlayRandomSound(SoundsDetectionStart, DetectionSoundVolume);
    }

    if (!bPlayedSuspicionSound && DetectionPercent >= SuspicionThreshold && PreviousPercent < SuspicionThreshold)
    {
        bPlayedSuspicionSound = true;
        PlayRandomSound(SoundsDetectionSuspicion, DetectionSoundVolume);
    }

    if (!bPlayedAlertSound && DetectionPercent >= AlertThreshold && PreviousPercent < AlertThreshold)
    {
        bPlayedAlertSound = true;
        PlayRandomSound(SoundsDetectionAlert, DetectionSoundVolume);
        
        if (bEnableBreathingSounds && !bIsBreathing)
        {
            StartBreathingSounds();
        }
    }

    if (DetectionPercent < SuspicionThreshold)
    {
        bPlayedStartSound = false;
        bPlayedSuspicionSound = false;
    }
    
    if (DetectionPercent < AlertThreshold)
    {
        bPlayedAlertSound = false;
        
        if (bIsBreathing)
        {
            StopBreathingSounds();
        }
    }
}

#pragma endregion

#pragma region Outline

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

#pragma endregion

#pragma region UI

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
        
        DetectionWidget->OnHalfDetectionReached.AddDynamic(this, &AGuardAIC::HandleHalfDetection);
        DetectionWidget->OnMaxDetectionReached.AddDynamic(this, &AGuardAIC::HandleMaxDetection);
        DetectionWidget->OnBlinkAnimationFinished.AddDynamic(this, &AGuardAIC::OnBlinkCompleted);
    }
}

void AGuardAIC::RemoveDetectionWidget()
{
    if (DetectionWidget)
    {
        DetectionWidget->ResetDetection();
        DetectionWidget->RemoveFromParent();
        DetectionWidget = nullptr;
    }
}

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

#pragma endregion

#pragma region Detection Events

void AGuardAIC::PlayerFullyDetected(AActor* Target)
{
   //TODO : Logique de gameplay lors de la détection complète du joueur
}

void AGuardAIC::OnBlinkCompleted()
{
    RemoveDetectionWidget();
}

void AGuardAIC::PlayerDetectionLost()
{
    RemoveDetectionWidget();
    
    PlayRandomSound(SoundsDetectionLost, DetectionSoundVolume);
    
    if (bIsBreathing)
    {
        StopBreathingSounds();
    }
    
    bPlayedStartSound = false;
    bPlayedSuspicionSound = false;
    bPlayedAlertSound = false;
    
    if (OutlineFadeDuration <= 0.f)
    {
        RemoveGuardOutline();
    }
    
    TrackedPlayer = nullptr;
}

#pragma endregion

#pragma region Audio System

void AGuardAIC::PlayRandomSound(const TArray<USoundBase*>& SoundArray, float Volume)
{
    if (SoundArray.Num() == 0)
    {
        return;
    }

    int32 RandomIndex = FMath::RandRange(0, SoundArray.Num() - 1);
    USoundBase* SelectedSound = SoundArray[RandomIndex];

    if (SelectedSound && GetPawn())
    {
        UGameplayStatics::SpawnSoundAtLocation(
            GetWorld(),
            SelectedSound,
            GetPawn()->GetActorLocation(),
            GetPawn()->GetActorRotation(),
            Volume,
            1.0f,
            0.0f, 
            nullptr, 
            nullptr,
            false   
        );
    }
}

void AGuardAIC::PlayRandomFootstep()
{
    PlayRandomSound(FootstepSounds, FootstepSoundVolume);
}

void AGuardAIC::PlayAmbientSound()
{
    if (!bEnableAmbientSounds || AmbientIdleSounds.Num() == 0)
    {
        return;
    }

    if (CurrentDetectionLevel == 0)
    {
        PlayRandomSound(AmbientIdleSounds, AmbientSoundVolume);
    }

    ScheduleNextAmbientSound();
}

void AGuardAIC::ScheduleNextAmbientSound()
{
    if (!bEnableAmbientSounds || AmbientIdleSounds.Num() == 0)
    {
        return;
    }

    float RandomDelay = FMath::RandRange(AmbientSoundMinInterval, AmbientSoundMaxInterval);

    GetWorldTimerManager().SetTimer(
        AmbientSoundTimerHandle,
        this,
        &AGuardAIC::PlayAmbientSound,
        RandomDelay,
        false
    );
}

void AGuardAIC::PlayBreathingSound()
{
    if (!bEnableBreathingSounds || BreathingSounds.Num() == 0)
    {
        return;
    }

    if (CurrentDetectionLevel >= AlertThreshold * MaxDetectionLevel)
    {
        PlayRandomSound(BreathingSounds, BreathingSoundVolume);
    }
}

void AGuardAIC::StartBreathingSounds()
{
    if (!bEnableBreathingSounds || BreathingSounds.Num() == 0 || bIsBreathing)
    {
        return;
    }

    bIsBreathing = true;

    float BreathingInterval = FMath::RandRange(2.0f, 4.0f);

    GetWorldTimerManager().SetTimer(
        BreathingSoundTimerHandle,
        this,
        &AGuardAIC::PlayBreathingSound,
        BreathingInterval,
        true
    );
}

void AGuardAIC::StopBreathingSounds()
{
    if (!bIsBreathing)
    {
        return;
    }

    bIsBreathing = false;

    if (GetWorldTimerManager().IsTimerActive(BreathingSoundTimerHandle))
    {
        GetWorldTimerManager().ClearTimer(BreathingSoundTimerHandle);
    }
}

#pragma endregion