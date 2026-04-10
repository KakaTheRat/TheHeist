#include "Pawn/SecurityCamera/MonitorSwipeData.h"
#include "Pawn/SecurityCamera/MonitorActor.h"

UMonitorSwipeData::UMonitorSwipeData()
{
	InteractText = "Swipe";
	bShouldAppearForThePlayer = false;
}

void UMonitorSwipeData::StartInteraction()
{
	AMonitorActor* Monitor = Cast<AMonitorActor>(OwnerActor);
	if (!Monitor || !Monitor->IsFocused())
	{
		EndOfInteraction();
		return;
	}

	bSwipeRight ? Monitor->SwipeNext() : Monitor->SwipePrevious();
	EndOfInteraction();
}