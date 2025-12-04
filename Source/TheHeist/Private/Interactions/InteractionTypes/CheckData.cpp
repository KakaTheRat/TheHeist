// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactions/InteractionTypes/CheckData.h"

#include "Camera/CameraComponent.h"

UCheckData::UCheckData()
{
	InteractText = "Check";
	
}

void UCheckData::StartInteraction()
{
	TArray<USceneComponent*> Components;
	OwnerActor->GetComponents<USceneComponent>(Components);
	if (!OwnerActor) return;
	for (USceneComponent* comp : Components)
	{
		if (comp->GetName() == LookTargetName)
		{
			LookTarget = comp;
			break;
		}
	}
	
	APlayerController* PC = Owner->GetWorld()->GetFirstPlayerController();
	if (!PC) { EndOfInteraction(); return; }

	APawn* PlayerPawn = PC->GetPawn();
	UCameraComponent* PlayerCamera = PlayerPawn->FindComponentByClass<UCameraComponent>();
	if (!PlayerCamera) { EndOfInteraction(); return; }
	
	OriginalCameraTransform = PlayerCamera->GetComponentTransform();


	if (LookTarget)
	{
		PlayerCamera->SetWorldLocationAndRotation(
			LookTarget->GetComponentLocation(),
			LookTarget->GetComponentRotation()
		);
	}


	if (UWorld* World = Owner->GetWorld())
	{
		World->GetTimerManager().SetTimer(
			CheckTimerHandle,
			[this, PlayerCamera]()
			{
	
				if (PlayerCamera)
				{
					PlayerCamera->SetWorldTransform(OriginalCameraTransform);
				}

				EndOfInteraction();
			},
			CheckDuration,
			false
		);
	}
}


