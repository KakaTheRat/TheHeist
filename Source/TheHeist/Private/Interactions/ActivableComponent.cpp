// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactions/ActivableComponent.h"
#include "GameFramework/Actor.h"


void UActivableComponent::Activate()
{
if (Owner)
{
	switch (CurrentInteractionData->activationType)
	{
		case EActivableType::Light:
			ActivateLight();
			break;

		case EActivableType::Sound:
			ActivateSound();
			break;
	}
}
}

void UActivableComponent::RunInteraction(USceneComponent* HitComponent, UInteractionData* Data)
{
	UE_LOG(LogTemp, Warning, TEXT("AH"));
	Super::RunInteraction(HitComponent, Data);
    
	UActivableData* ActivableData = Cast<UActivableData>(Data);
	if (!ActivableData)
	{
		UE_LOG(LogTemp, Warning, TEXT("RunInteraction: Wrong Data type!"));
		return;
	}
	// Stocker la Data typée
	CurrentInteractionData = ActivableData;
	CurrentlyChosenComponent = HitComponent;

	Activate();
}


void UActivableComponent::ActivateLight()
{
	UE_LOG(LogTemp, Warning, TEXT("5wi"));
	if (ULightComponent* LightComp = Owner->FindComponentByClass<ULightComponent>())
	{
		LightComp->SetIntensity(CurrentInteractionData->LightIntensity*!CurrentInteractionData->bIsActivated);
		CurrentInteractionData->bIsActivated = !CurrentInteractionData->bIsActivated;
	}
	
}

void UActivableComponent::ActivateSound()
{UE_LOG(LogTemp, Warning, TEXT("WW"));
	if (UAudioComponent* AudioComp = Owner->FindComponentByClass<UAudioComponent>())
	{
		UE_LOG(LogTemp, Warning, TEXT("1"));
		if (CurrentInteractionData->bIsActivated)
		{UE_LOG(LogTemp, Warning, TEXT("2"));
			AudioComp->Stop();
		}
		else
		{UE_LOG(LogTemp, Warning, TEXT("3 %p"),CurrentInteractionData->Sound );
			AudioComp->SetSound(CurrentInteractionData->Sound);
			AudioComp->Play();
			FTimerHandle NoiseTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				NoiseTimerHandle,
				this,	
				&UActivableComponent::ReportNoiseEvent,
				1.0f,   
				true   
			);
		}
	}
}

void UActivableComponent::ReportNoiseEvent()
{
	UE_LOG(LogTemp, Warning, TEXT("Mon message : Allume"));
	/*/if (Owner)
	{
		UAIPerceptionSystem::ReportNoiseEvent(
			GetWorld(),
			Owner->GetActorLocation(),
			1.0f,              
			Owner,              
			0.0f,
			TEXT("Interactable") 
		);
	}/*/
}