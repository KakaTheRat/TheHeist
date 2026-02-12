#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Curves/CurveFloat.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Perception/AIPerceptionComponent.h"

#include "SecurityCamera.generated.h"

class UAISenseConfig_Sight;
class UDetectionMeterWidget;
struct FAIStimulus;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHackCamera);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDetected);

UCLASS(Blueprintable)
class THEHEIST_API ASecurityCamera : public APawn
{
    GENERATED_BODY()

public:
    ASecurityCamera();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    bool bIsMoving = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (EditCondition = "bIsMoving"))
    float RotationSpeed = 30.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (EditCondition = "bIsMoving"))
    float MaxRotationAngle = 60.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
    int32 MaxDetectionLevel = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
    float DetectionTickDelay = 0.05f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
    float BlinkDuration = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Perception")
    float SightRadius = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Perception")
    float LoseSightRadius = 2500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Perception")
    float PeripheralVisionAngleDegrees = 45.0f; 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UDetectionMeterWidget> DetectionWidgetClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UAIPerceptionComponent* AIPerceptionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* RotationPivot;

    UPROPERTY(BlueprintAssignable, Category = "Camera")
    FOnHackCamera OnHackCamera;

    UPROPERTY(BlueprintAssignable, Category = "Detection")
    FOnPlayerDetected OnPlayerDetectedEvent;
    
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void HackCamera();
    
private:

    float CurrentRotation = 0.0f;
    bool bRotatingRight = true;
    bool bCameraHacked = false;
    FRotator InitialRotation;

    UPROPERTY()
    UAISenseConfig_Sight* SightConfig;

    int32 CurrentDetectionLevel = 0;
    bool bPlayerVisible = false;
    
    UPROPERTY()
    UDetectionMeterWidget* DetectionWidget;
    
    UPROPERTY()
    AActor* TrackedPlayer;
    
    FTimerHandle DetectionTimerHandle;

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
    
    UFUNCTION()
    void OnBlinkCompleted();
    
};