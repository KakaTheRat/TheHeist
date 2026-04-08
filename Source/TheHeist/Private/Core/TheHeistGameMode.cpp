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
	UE_LOG(LogTemp, Warning, TEXT("dqdqdzqor"));
	bIsGoodStatue = false;
	if (Statues.IsEmpty())
	{
		return;
	}
	for (auto Statue : Statues)
	{
		if (Statue->GetIsAvailableStatue())
		{
			if (!Statue->GetbIsGreateStatue())
			{
				bIsGoodStatue = false;
				break;
			}
			bIsGoodStatue = true;
		}
	}
}
