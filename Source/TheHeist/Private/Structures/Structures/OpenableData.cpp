#include "Structures/Interactions/OpenableData.h"

UOpenableData::UOpenableData()
{
	InteractText = "Open";
}

void UOpenableData::ExecuteInteraction(AActor* Owner, USceneComponent* Target)
{
	Super::ExecuteInteraction(Owner, Target);

	if (!Owner || !Target || !Curve) return;
	
	LinkedComponent = Target;
	
	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Black,
		FString::Printf(TEXT("Oué")));

	if (!bHasStoredInitialTransform)
	{
		InitialLocation = LinkedComponent->GetRelativeLocation();
		InitialRotation = LinkedComponent->GetRelativeRotation();
		bHasStoredInitialTransform = true;
	}
	if (!bTimelineInitialized)
	{
		InitTimeline(Owner);
		bTimelineInitialized = true;
	}
	// Si timeline déjà en cours, inverser
	if (Timeline.IsPlaying())
		Timeline.Reverse();
	else
	{
		if (bIsOpened)
			Timeline.ReverseFromEnd();
		else
			Timeline.PlayFromStart();
	}
	bIsOpened = !bIsOpened;
	InteractText = bIsOpened ? "Close" : "Open";
	// Find Timeline component on the actor
	
}
void UOpenableData::InitTimeline(AActor* Owner)
{
	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindUFunction(this, FName("HandleProgress"));
	Timeline.AddInterpFloat(Curve, ProgressFunction);

	// CORRECTION: Use FOnTimelineEventStatic instead of FOnTimelineEvent
	FOnTimelineEventStatic FinishedCallback;
	FinishedCallback.BindUFunction(this, FName("HandleFinished"));
	Timeline.SetTimelineFinishedFunc(FinishedCallback);

	Timeline.SetTimelineLength(Duration);
	Timeline.SetLooping(false);

	// Assure-toi que le tick de l'acteur est actif
	Owner->PrimaryActorTick.bCanEverTick = true;
}
void UOpenableData::HandleProgress(float Value)
{
	if (!LinkedComponent) return;

	switch (OpenableType)
	{
	case EOpenableType::Door:
		{
			float RotationAngle = (OpeningSide == EOpeningSide::Left || OpeningSide == EOpeningSide::Down) ? -Angle*Value : Angle*Value;
			if (OpeningSide == EOpeningSide::Up || OpeningSide == EOpeningSide::Down)
				LinkedComponent->SetRelativeRotation(FRotator(RotationAngle, 0, 0));
			else
				LinkedComponent->SetRelativeRotation(FRotator(0, RotationAngle, 0));
			break;
		}
	case EOpenableType::Drawer:
		{
			FVector Direction = FVector::ZeroVector;
			switch (OpeningSide)
			{
			case EOpeningSide::Right: Direction = FVector::RightVector; break;
			case EOpeningSide::Left: Direction = FVector::LeftVector; break;
			case EOpeningSide::Up: Direction = FVector::UpVector; break;
			case EOpeningSide::Down: Direction = FVector::DownVector; break;
			}
			LinkedComponent->SetRelativeLocation(InitialLocation + Direction * Distance * Value);
			break;
		}
	}
}

void UOpenableData::HandleFinished()
{
	EndOfInteraction();
	bHasStoredInitialTransform = false;
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