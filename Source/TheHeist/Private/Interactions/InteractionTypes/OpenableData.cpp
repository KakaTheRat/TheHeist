#include "Interactions/InteractionTypes/OpenableData.h"

#include "NetworkMessage.h"

UOpenableData::UOpenableData()
{
	InteractText = "Open";
	CurrentState = EOpeningStates::Close;
}

void UOpenableData::ExecuteInteraction(AActor* Owner, USceneComponent* Target, EInteractionContext Context, AActor* InteractingActor)
{
	Super::ExecuteInteraction(Owner, Target, Context, nullptr);

	CurrentInteractingActor = InteractingActor;
	 
	
	if (!Owner || !Target || !Curve) return;
	LinkedComponent = Target;

	if (!bTimelineInitialized)
	{
		InitTimeline(Owner);
		bTimelineInitialized = true;
	}
	if (!bHasStoredInitialTransform)
	{
		InitialLocation = LinkedComponent->GetRelativeLocation();
		InitialRotation = LinkedComponent->GetRelativeRotation();
		bHasStoredInitialTransform = true;
	}
	

	
	if (Timeline.IsPlaying())
	{
		Timeline.Reverse();
	}
	else
	{
		if (bIsOpened)
			Timeline.Reverse();
		else
			Timeline.PlayFromStart();
	}

	bIsOpened = !bIsOpened;
	InteractText = bIsOpened ? "Close" : "Open";
	
}
void UOpenableData::InitTimeline(AActor* Owner)
{
	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindUFunction(this, FName("HandleProgress"));
	Timeline.AddInterpFloat(Curve, ProgressFunction);

	
	FOnTimelineEventStatic FinishedCallback;
	FinishedCallback.BindUFunction(this, FName("HandleFinished"));
	Timeline.SetTimelineFinishedFunc(FinishedCallback);

	Timeline.SetTimelineLength(Duration);
	Timeline.SetLooping(false);

	
	Owner->PrimaryActorTick.bCanEverTick = true;
}
void UOpenableData::HandleProgress(float Value)
{
    if (!LinkedComponent) return;

    // Calculate rotation based of opening type
    switch (OpenableType)
    {
        case EOpeningType::Door:
        {
            // Determines opening side, based of opening side
            float RotationAngle = Angle * Value;
            switch (OpeningSide)
            {
                case EOpeningSide::Left:  RotationAngle = -RotationAngle; break;
                case EOpeningSide::Down:  RotationAngle = -RotationAngle; break;
                case EOpeningSide::Up:     break;
                case EOpeningSide::Right:  break;
                default: break;
            }

         
            FRotator DeltaRot = FRotator::ZeroRotator;
            if (OpeningSide == EOpeningSide::Up || OpeningSide == EOpeningSide::Down)
                DeltaRot.Pitch = RotationAngle;
            else
                DeltaRot.Yaw = RotationAngle;

            // Final rotation
            LinkedComponent->SetRelativeRotation(InitialRotation + DeltaRot);
            break;
        }

        case EOpeningType::Drawer:
        {
            // Calculate translation direction
            FVector Direction = FVector::ZeroVector;

            if (b_ShouldUseOpeningSide)
            {
                switch (OpeningSide)
                {
                    case EOpeningSide::Right: Direction = FVector::RightVector; break;
                    case EOpeningSide::Left:  Direction = -FVector::RightVector; break;
                    case EOpeningSide::Up:    Direction = FVector::UpVector; break;
                    case EOpeningSide::Down:  Direction = -FVector::UpVector; break;
                    default: break;
                }
            }
            else
            {
                Direction = FVector::ForwardVector;
            }

            // Pulling or pushing 
            float DirectionSign = (OpeningDirection == EOpeningDirection::Push) ? -1.f : 1.f;

            
            FVector RotatedDirection = InitialRotation.RotateVector(Direction);

            // Final translation
            LinkedComponent->SetRelativeLocation(InitialLocation + RotatedDirection * Distance * Value * DirectionSign);
            break;
        }
    }
}

void UOpenableData::HandleFinished()
{
	CurrentState = bIsOpened ? EOpeningStates::Open : EOpeningStates::Close;
	EndOfInteraction();
}

void UOpenableData::OnTimelineProgress(float Value)
{
	HandleProgress(Value);
}
void UOpenableData::Tick(float DeltaTime)
{
	if (Timeline.IsPlaying())
	{
		Timeline.TickTimeline(DeltaTime);
	}
}

TArray<FName> UOpenableData::GetAvailableStates()
{
	return InteractionHelpers::GetEnumNames<EOpeningStates>();
}


FName UOpenableData::GetCurrentState_Implementation() const
{
	return InteractionHelpers::GetEnumNameFromValue<EOpeningStates>(CurrentState);
}