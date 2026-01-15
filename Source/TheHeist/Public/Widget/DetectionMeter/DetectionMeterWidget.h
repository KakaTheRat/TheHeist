#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DetectionMeterWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHalfDetectionReached, FVector, PlayerLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxDetectionReached, AActor*, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBlinkAnimationFinished);

class UCanvasPanel;
class UProgressBar;
class UWidgetAnimation;
class UImage;

/**
 * UDetectionMeterWidget
 * 
 * Widget to display the detection meter for guards or security cameras.
 */
UCLASS()
class THEHEIST_API UDetectionMeterWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Detection")
    void UpdatePercent(float NewPercent);

    UFUNCTION(BlueprintCallable, Category = "Detection")
    void UpdateAngle(float AngleDegrees);

    UFUNCTION(BlueprintCallable, Category = "Detection")
    void BlinkIcon();

    UFUNCTION(BlueprintCallable, Category = "Detection")
    void SetPulseSpeed(float Speed);

    UPROPERTY(BlueprintAssignable, Category = "Detection")
    FOnHalfDetectionReached OnHalfDetectionReached;

    UPROPERTY(BlueprintAssignable, Category = "Detection")
    FOnMaxDetectionReached OnMaxDetectionReached;
    
    UPROPERTY(BlueprintAssignable, Category = "Detection")
    FOnBlinkAnimationFinished OnBlinkAnimationFinished;
    
    UFUNCTION(BlueprintCallable, Category = "Detection")
    void SetTrackedPlayer(AActor* Player);
    
    UFUNCTION(BlueprintCallable, Category = "Detection")
    void ResetDetection();

protected:

    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* RootCanvas;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* DetectionProgressBar;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* BlinkAnimation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Juice")
    float BasePulseSpeed = 1.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Juice")
    float PulseAmplitude = 0.15f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Juice")
    float ShakeIntensity = 5.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Juice")
    float ShakeFrequency = 10.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Juice")
    float BlinkAnimationDuration = 1.5f;

private:
    
    float CurrentPercent;
    float PreviousPercent;
    float TargetRotation;
    
    float PulseTime;
    float CurrentPulseSpeed;
    float CurrentGlowIntensity;
    FVector2D ShakeOffset;
    FVector2D BasePosition;
    
    void UpdatePulseEffect(float DeltaTime);
    void UpdateShakeEffect(float DeltaTime);

    bool bHalfDetectionTriggered;   
    
    UPROPERTY()
    AActor* TrackedPlayer;
    
    FTimerHandle BlinkFinishedTimerHandle;
    
    void OnBlinkFinished();
};