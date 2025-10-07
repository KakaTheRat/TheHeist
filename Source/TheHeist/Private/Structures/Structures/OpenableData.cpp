#include "Structures/Interactions/OpenableData.h"

UOpenableData::UOpenableData()
{
	InteractText = "Open";
}

void UOpenableData::ExecuteInteraction_Implementation(AActor* Owner, USceneComponent* Target)
{
	Super::ExecuteInteraction_Implementation(Owner, Target);
	LinkedComponent = Target;
	

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
        
		// Start timeline
		TimelineComp->PlayTimeline(Curve, Duration, bIsOpened);
		
	}

	// Set new state
	bIsOpened = !bIsOpened;
	InteractText = bIsOpened ? "Close" : "Open";
}

void UOpenableData::OnTimelineProgress(float Value)
{
	HandleProgress(Value);
}

void UOpenableData::HandleProgress(float Value)
{
	if (LinkedComponent)
	{
		// Animation
		FRotator NewRotation = FRotator(0.f, Value * 90.f, 0.f);
		LinkedComponent->SetRelativeRotation(NewRotation);
	}
}