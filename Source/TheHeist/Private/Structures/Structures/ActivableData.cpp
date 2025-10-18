#include "Structures/Interactions/ActivableData.h"

UActivableData::UActivableData()
{
	InteractText = "Activate";
}

void UActivableData::ExecuteInteraction(AActor* Owner, USceneComponent* Target)
{
	Super::ExecuteInteraction(Owner, Target);

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