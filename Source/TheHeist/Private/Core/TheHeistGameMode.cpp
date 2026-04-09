// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/TheHeistGameMode.h"
#include "Kismet/GameplayStatics.h"


ATheHeistGameMode::ATheHeistGameMode()
{
	// stub
}

void ATheHeistGameMode::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> Actors;

	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		AStatue::StaticClass(),
		Actors
	);

	for (AActor* Actor : Actors)
	{
		AStatue* Statue = Cast<AStatue>(Actor);
		Statue->OnActiveStatue.AddDynamic(this, &ATheHeistGameMode::VerrifyStatue);
		Statues.Add(Statue);
	}
}


void ATheHeistGameMode::VerrifyStatue()
{
	bIsGoodStatue = false;

	if (Statues.IsEmpty())
		return;

	// Réinitialise tous les flags "est regardée" avant la vérification globale
	for (AStatue* Statue : Statues)
	{
		if (Statue)
			Statue->SetIsBeingWatched(false);
	}

	// Vérifie les doublons : deux statues ne peuvent pas regarder la même cible
	TArray<AStatue*> WatchedTargets;
	for (AStatue* Statue : Statues)
	{
		if (!Statue || !Statue->GetIsAvailableStatue())
			continue;

		AStatue* Target = Statue->GetHitStatue();
		if (!Target)
			continue;

		if (WatchedTargets.Contains(Target))
		{
			UE_LOG(LogTemp, Warning, TEXT("Conflit : %s est déjà regardée par une autre statue !"),
				*Target->GetName());
			return; // Condition invalide, on sort
		}

		WatchedTargets.Add(Target);
		Target->SetIsBeingWatched(true);
	}

	// Vérification principale
	bIsGoodStatue = true;
	for (AStatue* Statue : Statues)
	{
		if (!Statue)
		{
			bIsGoodStatue = false;
			break;
		}

		if (!Statue->GetIsAvailableStatue())
		{
			continue;
		}

		// Doit avoir un hit valide
		if (!Statue->GetIsHitStatue())
		{
			UE_LOG(LogTemp, Warning, TEXT("%s ne regarde personne"), *Statue->GetName());
			bIsGoodStatue = false;
			break;
		}

	}

	if (bIsGoodStatue)
	{
		UE_LOG(LogTemp, Warning, TEXT("Statue victory "));
		OnAllActivatedStatue.Broadcast();
	}
}