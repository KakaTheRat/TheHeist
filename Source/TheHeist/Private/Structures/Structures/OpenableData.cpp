#include "Structures/Interactions/OpenableData.h"

UOpenableData::UOpenableData()
{
	InteractText = "Open";
}

void UOpenableData::ExecuteInteraction_Implementation(AActor* Owner, USceneComponent* Target)
{
	Super::ExecuteInteraction_Implementation(Owner, Target);
	LinkedComponent = Target;
	
	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Black,
		FString::Printf(TEXT("Oué")));
	// Find Timeline component on the actor
	UTimelinesComponent* TimelineComp = Owner->FindComponentByClass<UTimelinesComponent>();
    
	// If not found, creates a new one
	if (!TimelineComp)
	{
		TimelineComp = NewObject<UTimelinesComponent>(Owner);
		TimelineComp->RegisterComponent();
	}

	if (TimelineComp && Curve)
	{
		// Progress bind
		TimelineComp->OnTimelineProgress.AddDynamic(this, &UOpenableData::OnTimelineProgress);
		TimelineComp->OnTimelineFinished.AddDynamic(this, &UOpenableData::OnTimelineFinished);
        
		// Start timeline
		TimelineComp->PlayTimeline(Curve, Duration, bIsOpened);
		
	}

	// Set new state
	bIsOpened = !bIsOpened;
	InteractText = bIsOpened ? "Close" : "Open";
}

void UOpenableData::OnTimelineFinished()
{
	OnInteractionEnded.ExecuteIfBound();
}

void UOpenableData::OnTimelineProgress(float Value)
{
	HandleProgress(Value);
}

void UOpenableData::HandleProgress(float Value)
{
	GEngine->AddOnScreenDebugMessage(
	-1,
	3.f,
	FColor::Black,
	FString::Printf(TEXT("%f""Coucou"), Value));
	if (!LinkedComponent)
		return;

	FTransform InitialTransform = LinkedComponent->GetRelativeTransform();

	
	FVector Direction = FVector::ZeroVector;
	float RotationAngle = 0.f;
	
	switch (OpenableType)
	{
	case EOpenableType::Door:
		{
			//Rotation calculus
			switch (OpeningSide)
			{
			case EOpeningSide::Right:
				RotationAngle = Value * Angle;
				break;
			case EOpeningSide::Left:
				RotationAngle = Value * -Angle;
				break;
			case EOpeningSide::Up:
				RotationAngle = Value * Angle;
				LinkedComponent->SetRelativeRotation(FRotator(RotationAngle, 0.f, 0.f));
				return;
			case EOpeningSide::Down:
				RotationAngle = Value * -Angle;
				LinkedComponent->SetRelativeRotation(FRotator(RotationAngle, 0.f, 0.f));
				return;
			default:
				break;
			}

			//Rotation around Y axis
			LinkedComponent->SetRelativeRotation(FRotator(0.f, RotationAngle, 0.f));
			break;
		}

	case EOpenableType::Drawer:
		{
			// Translation Movement
			switch (OpeningSide)
			{
			case EOpeningSide::Right:
				Direction = FVector::RightVector;
				break;
			case EOpeningSide::Left:
				Direction = FVector::LeftVector;
				break;
			case EOpeningSide::Up:
				Direction = FVector::UpVector;
				break;
			case EOpeningSide::Down:
				Direction = FVector::DownVector;
				break;
			default:
				break;
			}
			
			FVector Offset = Direction * Value * Distance;

			LinkedComponent->SetRelativeLocation(InitialTransform.GetLocation() + Offset);
			break;
		}

	default:
		break;
	}
}