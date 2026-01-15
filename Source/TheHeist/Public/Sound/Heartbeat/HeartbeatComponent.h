#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HeartbeatComponent.generated.h"

class USoundBase;
class UAudioComponent;

/**
 * Component to manage the player's heartbeat sound based on stress level.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THEHEIST_API UHeartbeatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    
    UHeartbeatComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Heartbeat")
    void SetStressLevel(float NewStressLevel);

    UFUNCTION(BlueprintCallable, Category = "Heartbeat")
    void StartHeartbeat();

    UFUNCTION(BlueprintCallable, Category = "Heartbeat")
    void StopHeartbeat();

    UFUNCTION(BlueprintCallable, Category = "Heartbeat")
    void ResetSession();

protected:
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heartbeat|Audio")
    TArray<USoundBase*> HeartbeatSounds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heartbeat|Audio")
    USoundBase* SingleHeartbeatSound;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heartbeat|Rhythm", meta = (ClampMin = "0.3", ClampMax = "2.0"))
    float HeartbeatInterval;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heartbeat|Volume", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float MinVolume;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heartbeat|Volume", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float MaxVolume;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heartbeat|Volume", meta = (ClampMin = "0.1", ClampMax = "10.0"))
    float VolumeTransitionSpeed;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heartbeat|Pitch")
    bool bEnablePitchVariation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heartbeat|Pitch", meta = (EditCondition = "bEnablePitchVariation"))
    float PitchVariationRange;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heartbeat|Effects")
    bool bEnableDoubleBeat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heartbeat|Effects", meta = (EditCondition = "bEnableDoubleBeat"))
    float DoubleBeatDelay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heartbeat|Effects", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float HeartbeatActivationThreshold;

private:

    float CurrentStressLevel;
    float TargetStressLevel;

    float CurrentVolume;
    float TargetVolume;

    float TimeSinceLastBeat;

    bool bIsActive;

    bool bHasPlayedThisSession;

    UPROPERTY()
    UAudioComponent* AudioComponent;

    void PlayHeartbeat();
    void UpdateHeartbeatParameters(float DeltaTime);
};