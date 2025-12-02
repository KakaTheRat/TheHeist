// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Action_Whistle.h"
#include "Components/AudioComponent.h"

void UAction_Whistle::OnExecute_Implementation(AActor* OwningGuard)
{
	
	if (!OwningGuard || !f) return;

	UAudioComponent* AudioComp = UGameplayStatics::SpawnSoundAtLocation(
		OwningGuard,
		f,
		OwningGuard->GetActorLocation()
	);

	if (AudioComp)
	{
		AudioComp->OnAudioFinished.AddDynamic(this, &UAction_Whistle::OnActionEnded);

	}
}