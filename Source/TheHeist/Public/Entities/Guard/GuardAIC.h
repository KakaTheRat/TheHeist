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
    
    void PlayerFullyDetected();
    void PlayerDetectionLost();

    int32 CurrentDetectionLevel;
    bool bPlayerVisible;

    bool bHalfDetectionTriggered;
    
    
    UPROPERTY()
    AActor* TrackedPlayer;
    
    FTimerHandle DetectionTimerHandle;
};
