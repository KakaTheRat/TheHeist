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
    if (bWaitingForAnimation)
    {
        CurrentInteractingActor = InteractingActor;

        if (InteractionMontage && InteractingActor)
        {
            UAnimInstance* Anim = InteractingActor->FindComponentByClass<USkeletalMeshComponent>()->GetAnimInstance();

            if (Anim)
            {
                Anim->OnPlayMontageNotifyBegin.AddDynamic(this, &UInteractionData::OnMontageNotifyBegin);
                Anim->Montage_Play(InteractionMontage);
                bWaitingForAnimation = false;
            
                return;
            }
        }
    }
    
    
    
}

void UInteractionData::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
{
    ExecuteInteraction()
    if (!bWaitingForAnimation) return;
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, "Genial");
    // Filtrer le notify que tu veux écouter
    if (NotifyName == MontageNotifyToTrigger)
    {
        UE_LOG(LogTemp, Warning, TEXT("Notify reçu : OnInteract"));
        
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, "Super");

        // Si tu veux, tu peux arrêter d'attendre après
        bWaitingForAnimation = false;
    }
}





void UInteractionData::TriggerAlert(AActor* SourceActor, TSubclassOf<UAISense> Sense)
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
        StimulusSource->RegisterForSense(Sense);
       
    }
}

void UInteractionData::ClearAlert(AActor* SourceActor, TSubclassOf<UAISense> Sense)
{
    if (!SourceActor || !StimulusSource) return;

    // Désenregistre le composant du système de perception
    StimulusSource->UnregisterFromSense(Sense);

}


