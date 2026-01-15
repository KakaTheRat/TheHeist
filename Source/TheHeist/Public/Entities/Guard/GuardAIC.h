#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GuardAIC.generated.h"

class UDetectionMeterWidget;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
struct FAIStimulus;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMaxLevelStress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHalfDetection, FVector, PlayerLocation);

/**
 * Guard AI Controller class to manage guard behavior and detection of the player.
 */
UCLASS()
class THEHEIST_API AGuardAIC : public AAIController
{
    GENERATED_BODY()

public:
    
    AGuardAIC();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    void UpdatePlayerHeartbeat();

    UPROPERTY(BlueprintAssignable, Category = "Detection")
    FOnMaxLevelStress OnMaxLevelStress;

protected:
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
    int32 MaxDetectionLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
    float DetectionTickDelay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
    float BlinkDuration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
    float WidgetScreenOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
    float OutlineFadeDuration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float SuspicionThreshold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float AlertThreshold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Perception")
    float SightRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Perception")
    float LoseSightRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Perception")
    float PeripheralVisionAngleDegrees;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UDetectionMeterWidget> DetectionWidgetClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UAIPerceptionComponent* AIPerceptionComponent;

    UPROPERTY()
    UAISenseConfig_Sight* SightConfig;

    UPROPERTY(BlueprintReadOnly)
    UDetectionMeterWidget* DetectionWidget;

    UPROPERTY(BlueprintAssignable, Category = "Detection")
    FOnHalfDetection OnHalfDetection;

    UFUNCTION(BlueprintImplementableEvent)
    void HandleHalfDetection(FVector PlayerLocation);

    UFUNCTION(BlueprintImplementableEvent)
    void HandleMaxDetection(AActor* Target);

    UFUNCTION()
    void PlayerFullyDetected(AActor* Target);
    
    UPROPERTY(EditDefaultsOnly, Category = "Outline")
    UMaterialInterface* OutlineMaterialBase;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Detection")
    TArray<USoundBase*> SoundsDetectionStart;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Detection")
    TArray<USoundBase*> SoundsDetectionSuspicion;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Detection")
    TArray<USoundBase*> SoundsDetectionAlert;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Detection")
    TArray<USoundBase*> SoundsDetectionFull;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Detection")
    TArray<USoundBase*> SoundsDetectionLost;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Ambient")
    TArray<USoundBase*> AmbientIdleSounds;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Ambient")
    float AmbientSoundMinInterval = 8.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Ambient")
    float AmbientSoundMaxInterval = 20.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Ambient")
    bool bEnableAmbientSounds = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Footsteps")
    TArray<USoundBase*> FootstepSounds;
    
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayRandomFootstep();
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Breathing")
    TArray<USoundBase*> BreathingSounds;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Breathing")
    bool bEnableBreathingSounds = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Settings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float DetectionSoundVolume = 1.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Settings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float AmbientSoundVolume = 0.7f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Settings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float FootstepSoundVolume = 0.5f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Settings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float BreathingSoundVolume = 0.6f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Settings")
    class USoundAttenuation* CustomSoundAttenuation = nullptr;
    
private:
    
    void SetupPerception();
    
    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
    
    void OnPlayerDetected(AActor* DetectedPlayer);
    void OnPlayerLost();
    void DetectionLoop();
    
    void CreateDetectionWidget();
    void RemoveDetectionWidget();
    void UpdateWidgetAngle();
    
    void PlayerDetectionLost();
    
    UFUNCTION()
    void OnBlinkCompleted();
    
    int32 CurrentDetectionLevel;
    int32 PreviousDetectionLevel;
    bool bPlayerVisible;
    bool bHalfDetectionTriggered;
    
    UPROPERTY()
    AActor* TrackedPlayer;
    
    FTimerHandle DetectionTimerHandle;
    FTimerHandle OutlineFadeTimerHandle;
    FTimerHandle AmbientSoundTimerHandle;
    FTimerHandle BreathingSoundTimerHandle;

    UPROPERTY()
    UMaterialInstanceDynamic* OutlineMaterialInstance;
    
    void ApplyOutlineToGuard(FLinearColor OutlineColor, float OutlineWidth);
    void RemoveGuardOutline();
    void UpdateGuardOutline();
    
    void PlayRandomSound(const TArray<USoundBase*>& SoundArray, float Volume = 1.0f);
    
    void PlayAmbientSound();
    void ScheduleNextAmbientSound();
    
    void PlayBreathingSound();
    void StartBreathingSounds();
    void StopBreathingSounds();
    
    void CheckDetectionThresholds();
    
    bool bPlayedStartSound;
    bool bPlayedSuspicionSound;
    bool bPlayedAlertSound;
    bool bIsBreathing;
};