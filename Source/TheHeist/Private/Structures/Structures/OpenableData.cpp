#include "Structures/Interactions/OpenableData.h"

#include "NetworkMessage.h"

UOpenableData::UOpenableData()
{
	InteractText = "Open";
	CurrentState = EOpeningStates::Close;
}

void UOpenableData::ExecuteInteraction(AActor* Owner, USceneComponent* Target)
{
	Super::ExecuteInteraction(Owner, Target);

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

	switch (OpenableType)
	{
	case EOpeningType::Door:
		{
			float RotationAngle = (OpeningSide == EOpeningSide::Left || OpeningSide == EOpeningSide::Down) ? -Angle*Value : Angle*Value;
			if (OpeningSide == EOpeningSide::Up || OpeningSide == EOpeningSide::Down)
				LinkedComponent->SetRelativeRotation(FRotator(RotationAngle, 0, 0));
			else
				LinkedComponent->SetRelativeRotation(FRotator(0, RotationAngle, 0));
			break;
		}
	case EOpeningType::Drawer:
		FVector Direction = FVector::ZeroVector;

		if (b_ShouldUseOpeningSide)
		{
			
			switch (OpeningSide)
			{
			case EOpeningSide::Right: Direction = LinkedComponent->GetRightVector(); break;
			case EOpeningSide::Left:  Direction = -LinkedComponent->GetRightVector(); break;
			case EOpeningSide::Up:    Direction = LinkedComponent->GetUpVector(); break;
			case EOpeningSide::Down:  Direction = -LinkedComponent->GetUpVector(); break;
			default: break;
			}
		}
		else
		{
			
			Direction = LinkedComponent->GetForwardVector();
		}

		
		float DirectionSign = (OpeningDirection == EOpeningDirection::Push) ? -1.f : 1.f;

		LinkedComponent->SetRelativeLocation(InitialLocation + Direction * Distance * Value * DirectionSign);
		break;
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
	TArray<FName> States;

	// Récupère le type enum de EOpeningStates
	UEnum* EnumPtr = StaticEnum<EOpeningStates>();
	if (!EnumPtr)
		return States;

	// Parcourt toutes les valeurs de l’enum
	for (int32 i = 0; i < EnumPtr->NumEnums(); ++i)
	{
		// Ignore l’élément "_MAX" si tu en as un
		FString Name = EnumPtr->GetNameStringByIndex(i);
		if (!Name.Contains(TEXT("MAX")))
		{
			States.Add(FName(*Name));
		}
	}

	return States;
}




FName UOpenableData::GetCurrentState_Implementation() const
{
	switch (CurrentState)
	{
	case EOpeningStates::Open: return "Open";
	case EOpeningStates::Close: return "Close";
	default: return "Unknown";
	}
}