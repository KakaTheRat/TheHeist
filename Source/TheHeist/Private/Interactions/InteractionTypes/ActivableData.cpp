#include "Interactions/InteractionTypes/ActivableData.h"

UActivableData::UActivableData()
{
	InteractText = "ON";
}

void UActivableData::StartInteraction()
{
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

void UActivableData::ActivateLight(const AActor* m_Owner)
{
	if (!Owner) return;

	if (ULightComponent* LightComp = Owner->FindComponentByClass<ULightComponent>())
	{
		LightComp->SetIntensity(LightIntensity * !bIsActivated);
		bIsActivated ? InteractText = "ON" : InteractText = "OFF";
	}
	if (bIsActivated)
		{
			ClearAlert(OwnerActor, UAISense_Sight::StaticClass());
		}
	else
		{
			TriggerAlert(OwnerActor,UAISense_Sight::StaticClass());
		}
		bIsActivated = !bIsActivated;
	
}

void UActivableData::ActivateSound(const AActor* m_Owner)
{
	if (!Owner) return;

	if (UAudioComponent* AudioComp = Owner->FindComponentByClass<UAudioComponent>())
	{
		if (bIsActivated)
		{
			AudioComp->Stop();
			ClearAlert(OwnerActor, UAISense_Hearing::StaticClass());
		}
		else
		{
			AudioComp->SetSound(Sound);
			AudioComp->Play();
			UAISense_Hearing::ReportNoiseEvent(
			   GetWorld(),
			   Owner->GetActorLocation(), // Position du bruit
			   1.0f,                      // Loudness
			   Owner,                     // Instigator
			   1500.f                     // Range
		   );
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