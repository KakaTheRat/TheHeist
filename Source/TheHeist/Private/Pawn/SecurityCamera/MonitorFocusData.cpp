#include "Pawn/SecurityCamera/MonitorFocusData.h"
#include "Pawn/SecurityCamera/MonitorActor.h"
#include "Pawn/SecurityCamera/SurveillanceRoomActor.h"
#include "Kismet/GameplayStatics.h"

UMonitorFocusData::UMonitorFocusData()
{
	InteractText = "Watch";
}

void UMonitorFocusData::StartInteraction()
{
	AMonitorActor* Monitor = Cast<AMonitorActor>(OwnerActor);
	if (!Monitor)
	{
		EndOfInteraction();
		return;
	}

	AActor* RoomActor = UGameplayStatics::GetActorOfClass( Monitor->GetWorld(), ASurveillanceRoomActor::StaticClass());

	ASurveillanceRoomActor* Room = Cast<ASurveillanceRoomActor>(RoomActor);
	if (!Room)
	{
		EndOfInteraction();
		return;
	}

	Room->FocusMonitor(Monitor);
	EndOfInteraction();
}