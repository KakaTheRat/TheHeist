#include "Entities/Guard/GuardAIC.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Widget/DetectionMeter/DetectionMeterWidget.h"

#pragma region Initialize

AGuardAIC::AGuardAIC()
{
    PrimaryActorTick.bCanEverTick = true;
    
    MaxDetectionLevel = 100;
    DetectionTickDelay = 0.05f;
    BlinkDuration = 1.5f;
    WidgetScreenOffset = 300.0f;

    SightRadius = 2000.0f;
    LoseSightRadius = 2500.0f;
    PeripheralVisionAngleDegrees = 90.0f;

    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
    SetPerceptionComponent(*AIPerceptionComponent);

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

    CurrentDetectionLevel = 0;
    bPlayerVisible = false;
    DetectionWidget = nullptr;
    TrackedPlayer = nullptr;
}

void AGuardAIC::BeginPlay()
{
    Super::BeginPlay();
    SetupPerception();
}

#pragma endregion

/*
 * Setup AI Perception Component with Sight Sense
 */
void AGuardAIC::SetupPerception()
{
    if (!AIPerceptionComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("AIPerceptionComponent is NULL!"));
        return;
    }
    
    if (!SightConfig)
    {
        UE_LOG(LogTemp, Error, TEXT("SightConfig is NULL!"));
        return;
    }

    SightConfig->SightRadius = SightRadius;
    SightConfig->LoseSightRadius = LoseSightRadius;
    SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngleDegrees;
    SightConfig->SetMaxAge(5.0f);
    SightConfig->AutoSuccessRangeFromLastSeenLocation = 500.0f;
    
    UE_LOG(LogTemp, Warning, TEXT("Sight Config - Radius: %.2f, LoseRadius: %.2f, Angle: %.2f"), 
           SightRadius, LoseSightRadius, PeripheralVisionAngleDegrees);
    
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

    AIPerceptionComponent->ConfigureSense(*SightConfig);
    AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
    
    UE_LOG(LogTemp, Warning, TEXT("Perception Component Configured"));

    AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AGuardAIC::OnTargetPerceptionUpdated);
    UE_LOG(LogTemp, Warning, TEXT("Perception Event Bound - Setup Complete!"));
    
    FTimerHandle DebugTimer;
    GetWorldTimerManager().SetTimer(DebugTimer, [this]()
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC && PC->GetPawn())
        {
            APawn* PlayerPawn = PC->GetPawn();
            FVector PlayerLoc = PlayerPawn->GetActorLocation();
            FVector GuardLoc = GetPawn() ? GetPawn()->GetActorLocation() : FVector::ZeroVector;
            float Distance = FVector::Dist(PlayerLoc, GuardLoc);
            
            UE_LOG(LogTemp, Warning, TEXT("Player location: %s"), *PlayerLoc.ToString());
            UE_LOG(LogTemp, Warning, TEXT("Guard location: %s"), *GuardLoc.ToString());
            UE_LOG(LogTemp, Warning, TEXT("Distance to player: %.2f (Sight Radius: %.2f)"), Distance, SightRadius);
            
            UAIPerceptionStimuliSourceComponent* StimuliSource = PlayerPawn->FindComponentByClass<UAIPerceptionStimuliSourceComponent>();
            if (StimuliSource)
            {
                UE_LOG(LogTemp, Warning, TEXT("Player HAS AIPerceptionStimuliSourceComponent"));
                
                UE_LOG(LogTemp, Error, TEXT("FORCE REGISTERING PLAYER FOR SIGHT SENSE!"));
                StimuliSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
                StimuliSource->RegisterWithPerceptionSystem();
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Player MISSING AIPerceptionStimuliSourceComponent!"));
            }
            
            TArray<AActor*> PerceivedActors;
            AIPerceptionComponent->GetCurrentlyPerceivedActors(TSubclassOf<UAISense_Sight>(), PerceivedActors);
            UE_LOG(LogTemp, Warning, TEXT("Currently perceived actors: %d"), PerceivedActors.Num());
            
            if (PerceivedActors.Num() > 0)
            {
                for (AActor* Actor : PerceivedActors)
                {
                    UE_LOG(LogTemp, Display, TEXT("  -> Perceiving: %s"), *Actor->GetName());
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("NOT PERCEIVING ANY ACTORS!"));
            }
            
            TArray<AActor*> KnownActors;
            AIPerceptionComponent->GetKnownPerceivedActors(TSubclassOf<UAISense_Sight>(), KnownActors);
            UE_LOG(LogTemp, Warning, TEXT("Known perceived actors (ever seen): %d"), KnownActors.Num());
            
            AIPerceptionComponent->RequestStimuliListenerUpdate();
            UE_LOG(LogTemp, Warning, TEXT("Requested perception update"));
            UE_LOG(LogTemp, Warning, TEXT("========================================"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Could not find player!"));
        }
        
        FTimerHandle SecondCheckTimer;
        GetWorldTimerManager().SetTimer(SecondCheckTimer, [this]()
        {
            UE_LOG(LogTemp, Warning, TEXT(""));
            UE_LOG(LogTemp, Warning, TEXT("SECOND CHECK - 4 seconds after game start"));
            TArray<AActor*> PerceivedActors;
            AIPerceptionComponent->GetCurrentlyPerceivedActors(TSubclassOf<UAISense_Sight>(), PerceivedActors);
            UE_LOG(LogTemp, Warning, TEXT("Perceived actors now: %d"), PerceivedActors.Num());
            
            if (PerceivedActors.Num() > 0)
            {
                for (AActor* Actor : PerceivedActors)
                {
                    UE_LOG(LogTemp, Warning, TEXT("  -> NOW SEEING: %s"), *Actor->GetName());
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("STILL NOT SEEING ANYTHING - Check Blueprint settings!"));
                UE_LOG(LogTemp, Error, TEXT("Go to Player Blueprint -> AIPerceptionStimuliSource Component"));
                UE_LOG(LogTemp, Error, TEXT("Make sure 'Auto Register as Source' is checked for AI Sight!"));
            }
        }, 2.0f, false);
        
    }, 2.0f, false);
}

void AGuardAIC::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    UE_LOG(LogTemp, Display, TEXT("=== OnTargetPerceptionUpdated Called ==="));
    
    if (!Actor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Actor is NULL in OnTargetPerceptionUpdated"));
        return;
    }

    UE_LOG(LogTemp, Display, TEXT("Detected Actor: %s"), *Actor->GetName());
    UE_LOG(LogTemp, Display, TEXT("Stimulus - Successfully Sensed: %s"), Stimulus.WasSuccessfullySensed() ? TEXT("YES") : TEXT("NO"));

    APawn* DetectedPawn = Cast<APawn>(Actor);
    if (!DetectedPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("Actor is not a Pawn"));
        return;
    }

    UE_LOG(LogTemp, Display, TEXT("Detected Pawn: %s"), *DetectedPawn->GetName());

    APlayerController* PC = Cast<APlayerController>(DetectedPawn->GetController());
    if (!PC || !PC->IsPlayerController())
    {
        UE_LOG(LogTemp, Warning, TEXT("Pawn is not controlled by a PlayerController"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("PLAYER DETECTED/LOST!"));

    if (Stimulus.WasSuccessfullySensed())
    {
        UE_LOG(LogTemp, Warning, TEXT(">>> PLAYER IS NOW VISIBLE <<<"));
        OnPlayerDetected(Actor);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT(">>> PLAYER IS NOW LOST <<<"));
        OnPlayerLost();
    }
}

void AGuardAIC::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (DetectionWidget && IsValid(DetectionWidget))
    {
        UpdateWidgetAngle();
    }
}

void AGuardAIC::OnPlayerDetected(AActor* DetectedPlayer)
{
    if (!DetectedPlayer)
        return;

    UE_LOG(LogTemp, Warning, TEXT("OnPlayerDetected - Player: %s"), *DetectedPlayer->GetName());

    TrackedPlayer = DetectedPlayer;
    bPlayerVisible = true;

    if (!DetectionWidget || !IsValid(DetectionWidget))
    {
        UE_LOG(LogTemp, Display, TEXT("Creating Detection Widget"));
        CreateDetectionWidget();
    }

    if (!GetWorldTimerManager().IsTimerActive(DetectionTimerHandle))
    {
        UE_LOG(LogTemp, Display, TEXT("Starting Detection Timer"));
        GetWorldTimerManager().SetTimer(
            DetectionTimerHandle,
            this,
            &AGuardAIC::DetectionLoop,
            DetectionTickDelay,
            true
        );
    }
}

void AGuardAIC::OnPlayerLost()
{
    bPlayerVisible = false;
    UE_LOG(LogTemp, Warning, TEXT("OnPlayerLost - Starting decrement timer"));

    if (!GetWorldTimerManager().IsTimerActive(DetectionTimerHandle))
    {
        UE_LOG(LogTemp, Display, TEXT("Restarting Detection Timer for decrement"));
        GetWorldTimerManager().SetTimer(
            DetectionTimerHandle,
            this,
            &AGuardAIC::DetectionLoop,
            DetectionTickDelay,
            true
        );
    }
}

void AGuardAIC::DetectionLoop()
{
    if (bPlayerVisible)
    {
        CurrentDetectionLevel++;
        
        UE_LOG(LogTemp, Display, TEXT("Detection INCREASING: %d / %d (%.1f%%)"), 
               CurrentDetectionLevel, MaxDetectionLevel, 
               (float)CurrentDetectionLevel / MaxDetectionLevel * 100.0f);

        if (CurrentDetectionLevel >= MaxDetectionLevel)
        {
            CurrentDetectionLevel = MaxDetectionLevel;
            GetWorldTimerManager().ClearTimer(DetectionTimerHandle);
            UE_LOG(LogTemp, Error, TEXT("!!! PLAYER FULLY DETECTED !!!"));
            PlayerFullyDetected();
            return;
        }
    }
    else
    {
        CurrentDetectionLevel--;
        
        UE_LOG(LogTemp, Display, TEXT("Detection DECREASING: %d / %d (%.1f%%)"), 
               CurrentDetectionLevel, MaxDetectionLevel, 
               (float)CurrentDetectionLevel / MaxDetectionLevel * 100.0f);

        if (CurrentDetectionLevel <= 0)
        {
            CurrentDetectionLevel = 0;
            GetWorldTimerManager().ClearTimer(DetectionTimerHandle);
            UE_LOG(LogTemp, Warning, TEXT("Detection completely lost - Timer stopped"));
            PlayerDetectionLost();
            return;
        }
    }

    if (DetectionWidget && IsValid(DetectionWidget))
    {
        float Percent = static_cast<float>(CurrentDetectionLevel) / static_cast<float>(MaxDetectionLevel);
        DetectionWidget->UpdatePercent(Percent);
    }
}

void AGuardAIC::CreateDetectionWidget()
{
    if (!DetectionWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("DetectionWidgetClass is not set in GuardAIC!"));
        return;
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("Could not get PlayerController"));
        return;
    }

    DetectionWidget = CreateWidget<UDetectionMeterWidget>(PC, DetectionWidgetClass);
    if (DetectionWidget)
    {
        DetectionWidget->AddToViewport(100);
        DetectionWidget->SetVisibility(ESlateVisibility::Visible);
        DetectionWidget->UpdatePercent(0.0f);
        
        UE_LOG(LogTemp, Warning, TEXT("Detection Widget Created and Added to Viewport"));
        UE_LOG(LogTemp, Warning, TEXT("Widget Visibility: %s"), 
               DetectionWidget->IsVisible() ? TEXT("VISIBLE") : TEXT("HIDDEN"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create Detection Widget"));
    }
}

void AGuardAIC::RemoveDetectionWidget()
{
    if (DetectionWidget && IsValid(DetectionWidget))
    {
        DetectionWidget->RemoveFromParent();
        DetectionWidget = nullptr;
        UE_LOG(LogTemp, Display, TEXT("Detection Widget Removed"));
    }
}

void AGuardAIC::UpdateWidgetAngle()
{
    if (!DetectionWidget || !TrackedPlayer)
        return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC)
        return;

    APawn* PlayerPawn = PC->GetPawn();
    if (!PlayerPawn)
        return;

    FRotator PlayerRotation = PC->GetControlRotation();

    AActor* GuardActor = GetPawn();
    if (!GuardActor)
        return;

    FVector GuardLocation = GuardActor->GetActorLocation();
    FVector PlayerLocation = PlayerPawn->GetActorLocation();

    FRotator LookAtRotation = (GuardLocation - PlayerLocation).Rotation();

    FRotator DeltaRotation = LookAtRotation - PlayerRotation;
    
    float Yaw = FMath::UnwindDegrees(DeltaRotation.Yaw);

    DetectionWidget->UpdateAngle(Yaw);
}

void AGuardAIC::PlayerFullyDetected()
{
    if (DetectionWidget && IsValid(DetectionWidget))
    {
        DetectionWidget->BlinkIcon();
        UE_LOG(LogTemp, Display, TEXT("Blink animation triggered"));

        FTimerHandle RemoveWidgetTimer;
        GetWorldTimerManager().SetTimer(
            RemoveWidgetTimer,
            this,
            &AGuardAIC::RemoveDetectionWidget,
            BlinkDuration,
            false
        );
    }

    OnMaxLevelStress.Broadcast();
    UE_LOG(LogTemp, Warning, TEXT("OnMaxLevelStress Event Broadcasted"));
}

void AGuardAIC::PlayerDetectionLost()
{
    UE_LOG(LogTemp, Warning, TEXT("========================================"));
    UE_LOG(LogTemp, Warning, TEXT("    Player Detection Lost"));
    UE_LOG(LogTemp, Warning, TEXT("========================================"));

    RemoveDetectionWidget();

    TrackedPlayer = nullptr;
}