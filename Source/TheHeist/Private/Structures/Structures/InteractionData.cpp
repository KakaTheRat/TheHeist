#include "Structures/Interactions/InteractionData.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"

inline void UInteractionData::ExecuteInteraction_Implementation(AActor* Owner, USceneComponent* Target)
{
}

void UInteractionData::TriggerAlert(AActor* SourceActor)
{
    if (!bCanAlertGuards || !SourceActor) return;

    FVector Location = SourceActor->GetActorLocation();

    // Debug
    

    if (!StimulusSource)
    {
        StimulusSource = SourceActor->FindComponentByClass<UAIPerceptionStimuliSourceComponent>();
        StimulusSource->RegisterComponent();
        
    }
    
    if (StimulusSource)
    {
        
        // Registers stimulus to the sense
        StimulusSource->RegisterForSense(UAISense_Sight::StaticClass());
       
    }
}

void UInteractionData::ClearAlert(AActor* SourceActor)
{
    if (!SourceActor || !StimulusSource) return;

    // Désenregistre le composant du système de perception
    StimulusSource->UnregisterFromSense(UAISense_Sight::StaticClass());

    // Debug
    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, TEXT("stop  "));
}
/*void UOpenableData::ExecuteInteraction_Implementation(AActor* Owner, USceneComponent* Target)
{
    Super::ExecuteInteraction_Implementation(Owner, Target);
    LinkedComponent = Target;

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("a!"));

    // Essaie d'abord avec la Timeline normale
    if (Curve)
    {
        FOnTimelineFloat ProgressFunction;
        ProgressFunction.BindUFunction(this, FName("HandleProgress"));
        
        Timeline = FTimeline(); 
        Timeline.AddInterpFloat(Curve, ProgressFunction);
        Timeline.SetTimelineLength(Duration);
        Timeline.SetLooping(false);
    }

    if (bIsOpened)
    {
        InteractText = "Close";
        PlayReverse();
        
        // Fallback manuel si la timeline ne marche pas
        if (!Timeline.IsPlaying())
        {
            bManualAnimating = true;
            bManualReverse = true;
            ManualProgress = 1.0f;
        }
    }
    else
    {
        InteractText = "Open";
        PlayForward();
        
        // Fallback manuel si la timeline ne marche pas
        if (!Timeline.IsPlaying())
        {
            bManualAnimating = true;
            bManualReverse = false;
            ManualProgress = 0.0f;
        }
    }
    bIsOpened = !bIsOpened;
}

void UOpenableData::PlayForward()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("b!"));
    if (Curve)
    {
        Timeline.PlayFromStart();
    }
}

void UOpenableData::PlayReverse()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("c!"));
    if (Curve)
    {
        Timeline.ReverseFromEnd();
    }
}

void UOpenableData::HandleProgress(float Value)
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Ho!"));
    if (LinkedComponent)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Hello Screen!"));
        FRotator NewRotation = FRotator(0.f, Value * 90.f, 0.f);
        LinkedComponent->SetRelativeRotation(NewRotation);
    }
}

/*void UOpenableData::Tick(float DeltaTime)
{
    // Essaie d'abord la Timeline normale
    if (Timeline.IsPlaying())
    {
        Timeline.TickTimeline(DeltaTime);
        GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, TEXT("Timeline Playing"));
    }
    // Fallback manuel si la timeline ne marche pas
    else if (bManualAnimating && LinkedComponent)
    {
        // Met à jour la progression manuellement
        if (bManualReverse)
        {
            ManualProgress -= DeltaTime / Duration;
            if (ManualProgress <= 0.0f)
            {
                ManualProgress = 0.0f;
                bManualAnimating = false;
            }
        }
        else
        {
            ManualProgress += DeltaTime / Duration;
            if (ManualProgress >= 1.0f)
            {
                ManualProgress = 1.0f;
                bManualAnimating = false;
            }
        }

        // Applique la courbe
        float AnimatedProgress = ManualProgress;
        if (Curve)
        {
            AnimatedProgress = Curve->GetFloatValue(ManualProgress);
        }

        // Appelle HandleProgress
        HandleProgress(AnimatedProgress);

        GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, 
            FString::Printf(TEXT("Manual Progress: %.2f"), AnimatedProgress));
    }
}*/

