#include "Interactions/InteractionTypes/InteractionData.h"

#include "Entities/EntitiesInterface.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Entities/Player/PlayerInteractionComponent.h"

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



void UInteractionData::ExecuteInteraction(AActor* m_Owner, USceneComponent* m_Target, EInteractionContext m_Context, AActor* m_InteractingActor)
{
    Owner = m_Owner;
    Target = m_Target;
    Context = m_Context;
    InteractingActor = m_InteractingActor;
    
    if (InteractionMontage)
    {
        if (InPosition != "none")
            
        {
            TArray<USceneComponent*> Components;
            Owner->GetComponents<USceneComponent>(Components);
            for (USceneComponent* s : Components)
            {
                if (s->GetName() == InPosition)
                {
                    IEntitiesInterface::Execute_MoveEntity(InteractingActor, s);
                }
            }
        }
        
       PlayAnimation();
        return;
    }
    StartInteraction();
}

void UInteractionData::StartInteraction()
{
    
}


void UInteractionData::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
{
    if (NotifyName == MontageNotifyToTrigger)
    {
        ACharacter* Character = Cast<ACharacter>(InteractingActor);

       UPlayerInteractionComponent* f = Character->GetComponentByClass<UPlayerInteractionComponent>();

        f->bEnableHandIK = false;
        f->InteractionTarget = nullptr;
        
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, "Super");
        
        StartInteraction();
    }
    if (NotifyName == "Start")
    {
        ACharacter* Character = Cast<ACharacter>(InteractingActor);

        UPlayerInteractionComponent* f = Character->GetComponentByClass<UPlayerInteractionComponent>();

        f->bEnableHandIK = true;

        TArray<USceneComponent*> Components;
        Owner->GetComponents<USceneComponent>(Components);
        
        for (USceneComponent* s : Components)
        {
            if (s->GetName() == AnimationTarget)
            {
                f->InteractionTarget = s;
                GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, "YAHEE");
                return;
            }
        }
        
        
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


void UInteractionData::PlayAnimation()
{
    if (InteractionMontage && InteractingActor)
    {
        USkeletalMeshComponent* Mesh = IEntitiesInterface::Execute_GetSkeletalMeshComponent(InteractingActor);
        
        UAnimInstance* Anim = Mesh->GetAnimInstance();


        if (Anim)
        {
            Anim->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UInteractionData::OnMontageNotifyBegin);
            Anim->OnPlayMontageNotifyBegin.AddDynamic(this, &UInteractionData::OnMontageNotifyBegin);
            Anim->Montage_Play(InteractionMontage, PlayRate);
        }
    }
}

