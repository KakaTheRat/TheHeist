#include "Structures/Interactions/OpenableData.h"

UOpenableData::UOpenableData()
{
	InteractText = "Open";
}

void UOpenableData::ExecuteInteraction_Implementation(AActor* Owner, USceneComponent* Target)
{
	GEngine->AddOnScreenDebugMessage(1, 5, FColor::Red, "AA");
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
        
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, 
			bIsOpened ? TEXT("🔙 Timeline Reverse") : TEXT("🔛 Timeline Forward"));
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
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Magenta, 
		FString::Printf(TEXT("🎬 Progress: %.2f"), Value));
    
	if (LinkedComponent)
	{
		// Animation
		FRotator NewRotation = FRotator(0.f, Value * 90.f, 0.f);
		LinkedComponent->SetRelativeRotation(NewRotation);
	}
}