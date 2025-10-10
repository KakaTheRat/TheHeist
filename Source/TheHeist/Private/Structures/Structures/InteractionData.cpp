#include "Structures/Interactions/InteractionData.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"

inline void UInteractionData::ExecuteInteraction(AActor* Owner, USceneComponent* Target)
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


