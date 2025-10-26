// Fill out your copyright notice in the Description page of Project Settings.


#include "Structures/Interactions/CheckData.h"

#include "Camera/CameraComponent.h"

UCheckData::UCheckData()
{
	InteractText = "Check";
	
}

void UCheckData::ExecuteInteraction(AActor* Owner, USceneComponent* Target, EInteractionContext Context, AActor* InteractingActor)
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
	Super::ExecuteInteraction(Owner, Target, Context, nullptr);
	if (!Owner) return;
	APlayerController* PC = Owner->GetWorld()->GetFirstPlayerController();
	if (!PC) { EndOfInteraction(); return; }

	APawn* PlayerPawn = PC->GetPawn();
	UCameraComponent* PlayerCamera = PlayerPawn->FindComponentByClass<UCameraComponent>();
	if (!PlayerCamera) { EndOfInteraction(); return; }
	// 2️⃣ Sauvegarde la position et rotation actuelle
	OriginalCameraTransform = PlayerCamera->GetComponentTransform();

	// 3️⃣ Positionne la caméra sur le point d'observation
	if (LookTarget)
	{
		PlayerCamera->SetWorldLocationAndRotation(
			LookTarget->GetComponentLocation(),
			LookTarget->GetComponentRotation()
		);
	}

	// 4️⃣ Timer pour la durée du check
	if (UWorld* World = Owner->GetWorld())
	{
		World->GetTimerManager().SetTimer(
			CheckTimerHandle,
			[this, PlayerCamera]()
			{
				// Restore la caméra à sa position originale
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


