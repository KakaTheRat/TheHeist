#include "Interactions/InteractionTypes/ActivableData.h"

UActivableData::UActivableData()
{
	InteractText = "Activate";
}

void UActivableData::ExecuteInteraction(AActor* Owner, USceneComponent* Target, EInteractionContext Context, AActor* InteractingActor)
{
	Super::ExecuteInteraction(Owner, Target, Context, nullptr);

	switch (ActivationType)
	{
	case EActivableType::Light:
		ActivateLight(Owner);
		break;

	case EActivableType::Sound:
		ActivateSound(Owner);
		break;

	default:
		break;
	}

	EndOfInteraction();
}

void UActivableData::ActivateLight(AActor* Owner)
{
	if (!Owner) return;

	if (ULightComponent* LightComp = Owner->FindComponentByClass<ULightComponent>())
	{
		LightComp->SetIntensity(LightIntensity * !bIsActivated);
		bIsActivated = !bIsActivated;
		bIsActivated ? InteractText = "Activated" : InteractText = "deaActivated";
	}

	
}

void UActivableData::ActivateSound(AActor* Owner)
{
	if (!Owner) return;

	if (UAudioComponent* AudioComp = Owner->FindComponentByClass<UAudioComponent>())
	{
		if (bIsActivated)
		{
			AudioComp->Stop();
			ClearAlert(Owner);
		}
		else
		{
			AudioComp->SetSound(Sound);
			AudioComp->Play();
			TriggerAlert(Owner);
			UGameplayStatics::SpawnSoundAttached(Sound, AudioComp);
		}
		bIsActivated = !bIsActivated;
		
		

		
	}
}

TArray<FName> UActivableData::GetAvailableStates()
{
	TArray<FName> AvailableStates;

	UE_LOG(LogTemp, Display, TEXT("Wtf") );
	AvailableStates.AddUnique(FName("O"));
	AvailableStates.AddUnique(FName("F"));
	
	return AvailableStates;
}