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
    if (InteractingActor && InteractingActor->GetClass()->ImplementsInterface(UEntitiesInterface::StaticClass()))
    {
        UAnimInstance* Anim = IEntitiesInterface::Execute_GetSkeletalMeshComponent(InteractingActor)->GetAnimInstance();
        if (Anim)
        {
            Anim->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UInteractionData::OnMontageNotifyBegin);
        }
    }

    
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

    InteractingActor = nullptr;
    
}



void UInteractionData::ExecuteInteraction(AActor* m_Owner, USceneComponent* m_Target, EInteractionContext m_Context, AActor* m_InteractingActor)
{
    Owner = m_Owner;
    Target = m_Target;
    Context = m_Context;
    InteractingActor = m_InteractingActor;

    USceneComponent* In = nullptr;
    USceneComponent* Out = nullptr;

    if (OutPosition != "none" || InPosition != "none")
    {
       
        TArray<USceneComponent*> Components;
        Owner->GetComponents<USceneComponent>(Components);

        for (USceneComponent* Comp : Components)
        {
            if (Comp->GetName() == InPosition)
            {
                In = Comp;
            }
            if (Comp->GetName() == OutPosition)
            {
                Out = Comp;
            }
        }

        FVector PlayerLoc = InteractingActor->GetActorLocation();

        USceneComponent* BestPoint = nullptr;

    
        if (In && Out)
        {
            float InDist = FVector::Dist(PlayerLoc, In->GetComponentLocation());
            float OutDist = FVector::Dist(PlayerLoc, Out->GetComponentLocation());

            BestPoint = (InDist <= OutDist) ? In : Out;
        }
        else if (In) BestPoint = In;
        else if (Out) BestPoint = Out;

            
        if (BestPoint)
        {
            USceneComponent* Targ = nullptr;
            TArray<USceneComponent*> Componentss;
            Owner->GetComponents<USceneComponent>(Componentss);
        
            for (USceneComponent* s : Componentss)
            {
                if (s->GetName() == AnimationTarget)
                {
                    Targ = s;
                }
            }

            if (InteractingActor->GetClass()->ImplementsInterface(UEntitiesInterface::StaticClass()))
            {
                IEntitiesInterface::Execute_MoveAndLookEntity(InteractingActor, BestPoint, Targ );
            }
        }
    }
        
    if (PlayAnimation(AnimationMontageToPlay()))
    {
        return;
    }
    
    StartInteraction();
}

void UInteractionData::StartInteraction()
{
    
}


AActor* UInteractionData::GetEffectiveActor() const
{
   if (bUseExternalActor && ExternalActor)
            return ExternalActor;

        return OwnerActor;
}

void UInteractionData::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
{
    if (NotifyName == MontageNotifyToTrigger)
    {
        ACharacter* Character = Cast<ACharacter>(InteractingActor);

       UPlayerInteractionComponent* f = Character->GetComponentByClass<UPlayerInteractionComponent>();

        f->bEnableHandIK = false;
        f->InteractionTarget = nullptr;
        
        UE_LOG(LogTemp, Warning, TEXT("Interaction class: %s"),
    *GetClass()->GetName());

        UE_LOG(LogTemp, Warning, TEXT("Interaction address: %p"), this);
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
                f->TargetTransform = s->GetComponentTransform();
                
                return;
            }
        }
        
        
    }
}

void UInteractionData::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (InteractingActor && InteractingActor->GetClass()->ImplementsInterface(UEntitiesInterface::StaticClass()))
    {
        IEntitiesInterface::Execute_SetAnimationState(InteractingActor, false);
        
        UAnimInstance* Anim = IEntitiesInterface::Execute_GetSkeletalMeshComponent(InteractingActor)->GetAnimInstance();
        if (Anim)
        {
       //     Anim->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UInteractionData::OnMontageNotifyBegin);
         //   Anim->OnMontageEnded.RemoveDynamic(this, &UInteractionData::OnMontageEnded);
        }
    }
}

UAnimMontage* UInteractionData::AnimationMontageToPlay()
{
    return InteractionMontage;
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

   
    StimulusSource->UnregisterFromSense(Sense);

}


bool UInteractionData::PlayAnimation(UAnimMontage* Animation)
{
    if (Animation && InteractingActor && InteractingActor->GetClass()->ImplementsInterface(UEntitiesInterface::StaticClass()))
    {
        USkeletalMeshComponent* Mesh = IEntitiesInterface::Execute_GetSkeletalMeshComponent(InteractingActor);
        
        UAnimInstance* Anim = Mesh->GetAnimInstance();


        if (Anim)
        {
            if (Anim->IsAnyMontagePlaying())
            {
                Anim->StopAllMontages(0.1f);
            }
            
            IEntitiesInterface::Execute_SetAnimationState(InteractingActor, true);

            Anim->OnPlayMontageNotifyBegin.Clear(); 
            Anim->OnMontageEnded.Clear();
            
            Anim->OnPlayMontageNotifyBegin.AddDynamic(this, &UInteractionData::OnMontageNotifyBegin);
            Anim->OnMontageEnded.AddDynamic(this, &UInteractionData::OnMontageEnded);
            
            Anim->Montage_Play(Animation, PlayRate);

            return true;
        }

        
    }
    return false;
}

