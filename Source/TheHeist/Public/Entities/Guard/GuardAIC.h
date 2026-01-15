#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GuardAIC.generated.h"

class UDetectionMeterWidget;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
struct FAIStimulus;

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
    
    int32 CurrentDetectionLevel;
    bool bPlayerVisible;
    bool bHalfDetectionTriggered;
    
    UPROPERTY()
    AActor* TrackedPlayer;
    
    FTimerHandle DetectionTimerHandle;
    FTimerHandle OutlineFadeTimerHandle;

    UPROPERTY()
    UMaterialInstanceDynamic* OutlineMaterialInstance;
    
    void ApplyOutlineToGuard(FLinearColor OutlineColor, float OutlineWidth);
    void RemoveGuardOutline();
    void UpdateGuardOutline();
};