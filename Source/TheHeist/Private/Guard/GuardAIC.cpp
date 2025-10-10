// Fill out your copyright notice in the Description page of Project Settings.


#include "Guard/GuardAIC.h"
#include "Player/PlayerInteractionComponent.h"


void AGuardAIC::BeginPlay()
{
	Super::BeginPlay();
	
	ATheHeistCharacter* Player = Cast<ATheHeistCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!Player) return;

	
	UPlayerInteractionComponent* AlertComp = Player->FindComponentByClass<UPlayerInteractionComponent>();
	if (!AlertComp) return;
	
	AlertComp->OnPlayerShout.AddUObject(this, &AGuardAIC::OnPlayerShout);
}
void AGuardAIC::OnPlayerShout(FVector Location)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Location.ToString());
}

