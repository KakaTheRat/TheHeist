#include "Structures/Interactions/ActivableData.h"

UActivableData::UActivableData()
{
	InteractText = "Activate";
}

void UActivableData::ExecuteInteraction_Implementation(AActor* Owner, USceneComponent* Target)
{
	Super::ExecuteInteraction_Implementation(Owner, Target);

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
}

void UActivableData::ActivateLight(AActor* Owner)
{
	if (!Owner) return;

	if (ULightComponent* LightComp = Owner->FindComponentByClass<ULightComponent>())
	{
		LightComp->SetIntensity(LightIntensity * !bIsActivated);
		bIsActivated = !bIsActivated;
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
		}
		else
		{
			AudioComp->SetSound(Sound);
			AudioComp->Play();
			UGameplayStatics::SpawnSoundAttached(Sound, AudioComp);
		}
		bIsActivated = !bIsActivated;
	}
}