#include "Sound/Heartbeat/HeartbeatComponent.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"

UHeartbeatComponent::UHeartbeatComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    HeartbeatInterval = 0.8f;

    MinVolume = 0.1f;
    MaxVolume = 0.8f;
    VolumeTransitionSpeed = 2.0f;

    bEnablePitchVariation = false; 
    PitchVariationRange = 0.05f;

    bEnableDoubleBeat = false; 
    DoubleBeatDelay = 0.15f;
    HeartbeatActivationThreshold = 0.1f;

    CurrentStressLevel = 0.0f;
    TargetStressLevel = 0.0f;
    CurrentVolume = MinVolume;
    TargetVolume = MinVolume;
    TimeSinceLastBeat = 0.0f;
    bIsActive = false;
    bHasPlayedThisSession = false;

    AudioComponent = nullptr;
    SingleHeartbeatSound = nullptr;
}

void UHeartbeatComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!AudioComponent)
    {
        AudioComponent = NewObject<UAudioComponent>(GetOwner(), TEXT("HeartbeatAudioComponent"));
        if (AudioComponent)
        {
            AudioComponent->RegisterComponent();
            AudioComponent->bAutoActivate = false;
            AudioComponent->bStopWhenOwnerDestroyed = true;
            AudioComponent->bIsUISound = true;
        }
    }
}

void UHeartbeatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bIsActive)
    {
        return;
    }

    UpdateHeartbeatParameters(DeltaTime);

    if (AudioComponent)
    {
        AudioComponent->SetVolumeMultiplier(CurrentVolume);
    }

    TimeSinceLastBeat += DeltaTime;

    if (TimeSinceLastBeat >= HeartbeatInterval)
    {
        if (CurrentStressLevel >= HeartbeatActivationThreshold)
        {
            PlayHeartbeat();
        }
        
        TimeSinceLastBeat = 0.0f;
    }
}

void UHeartbeatComponent::SetStressLevel(float NewStressLevel)
{
    TargetStressLevel = FMath::Clamp(NewStressLevel, 0.0f, 1.0f);

    TargetVolume = FMath::Lerp(MinVolume, MaxVolume, TargetStressLevel);

    if (TargetStressLevel >= HeartbeatActivationThreshold && !bHasPlayedThisSession)
    {
        StartHeartbeat();
    }
    else if (TargetStressLevel < HeartbeatActivationThreshold && bIsActive)
    {
        StopHeartbeat();
    }
}

void UHeartbeatComponent::StartHeartbeat()
{
    if (bIsActive || bHasPlayedThisSession)
    {
        return;
    }

    bIsActive = true;
    bHasPlayedThisSession = true;
    TimeSinceLastBeat = 0.0f;
}

void UHeartbeatComponent::StopHeartbeat()
{
    bIsActive = false;
    TimeSinceLastBeat = 0.0f;

    if (AudioComponent && AudioComponent->IsPlaying())
    {
        AudioComponent->FadeOut(0.5f, 0.0f);
    }
    
}

void UHeartbeatComponent::PlayHeartbeat()
{
    if (AudioComponent && AudioComponent->IsPlaying())
    {
        return;
    }

    USoundBase* SoundToPlay = nullptr;

    if (HeartbeatSounds.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, HeartbeatSounds.Num() - 1);
        SoundToPlay = HeartbeatSounds[RandomIndex];
    }
    else if (SingleHeartbeatSound)
    {
        SoundToPlay = SingleHeartbeatSound;
    }

    if (!SoundToPlay || !AudioComponent)
    {
        return;
    }

    AudioComponent->SetSound(SoundToPlay);
    AudioComponent->SetVolumeMultiplier(CurrentVolume);
    AudioComponent->SetPitchMultiplier(1.0f);
    AudioComponent->Play();
}

void UHeartbeatComponent::UpdateHeartbeatParameters(float DeltaTime)
{
    CurrentStressLevel = FMath::FInterpTo(
        CurrentStressLevel,
        TargetStressLevel,
        DeltaTime,
        VolumeTransitionSpeed
    );

    CurrentVolume = FMath::FInterpTo(
        CurrentVolume,
        TargetVolume,
        DeltaTime,
        VolumeTransitionSpeed
    );
}

void UHeartbeatComponent::ResetSession()
{
    bHasPlayedThisSession = false;
    bIsActive = false;
    CurrentStressLevel = 0.0f;
    TargetStressLevel = 0.0f;
    TimeSinceLastBeat = 0.0f;
    
    if (AudioComponent && AudioComponent->IsPlaying())
    {
        AudioComponent->Stop();
    }
}