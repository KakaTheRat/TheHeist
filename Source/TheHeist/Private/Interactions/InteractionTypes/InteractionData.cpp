#include "Interactions/InteractionTypes/InteractionData.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"

TArray<FName> UInteractionData::GetAvailableStates()
{
    TArray<FName> AvailableStates;
    return AvailableStates;
}

void UInteractionData::EndOfInteraction()
{
    OnInteractionEnded.Broadcast(CurrentInteractingActor, this);
}

//Determines the actor owning the component instanciating this object. 
void UInteractionData::PostInitProperties()
{
    Super::PostInitProperties();

    if (!HasAnyFlags(RF_ClassDefaultObject))
    {
        if (const UActorComponent* Comp = Cast<UActorComponent>(GetOuter()))
        {
            OwnerActor = Comp->GetOwner();
        }
    }
}

void UInteractionData::ExecuteInteraction(AActor* Owner, USceneComponent* Target, EInteractionContext Context, AActor* InteractingActor)
{
    CurrentInteractingActor = InteractingActor;
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

}


