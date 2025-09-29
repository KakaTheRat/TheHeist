// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactions/HideableComponent.h"




void UHideableComponent::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	PlayerRef = PC->GetPawn();
	
}

void UHideableComponent::RunInteraction(USceneComponent* HitComponent, UInteractionData* Data)
{
	
	Super::RunInteraction(HitComponent, Data);
	UE_LOG(LogTemp, Warning, TEXT("1!"));
	UHideableData* HideableData = Cast<UHideableData>(Data);
	if (!HideableData)
	{
		UE_LOG(LogTemp, Warning, TEXT("RunInteraction: Wrong Data type!"));
		return;
	}

	// Stocker la Data typée
	CurrentInteractionData = HideableData;
	CurrentlyChosenComponent = HitComponent;

	TArray<USceneComponent*> Components;
	Owner->GetComponents<USceneComponent>(Components);

	for (USceneComponent* Comp : Components)
	{
		if (!Comp) continue;

		if (Comp->GetName() == CurrentInteractionData->HiddenArrowName)
		{
			HiddenLocation = Comp->GetComponentLocation();
		}
		else if (Comp->GetName() == CurrentInteractionData->QuitArrowName) 
		{
			QuitLocation = Comp->GetComponentLocation();
		}
	}
	
	HidingInteract();
}
void UHideableComponent::HidingInteract()
{UE_LOG(LogTemp, Warning, TEXT("2"));
	PlayerComp = PlayerRef->FindComponentByClass<UPlayerInteractionComponent>();
	
	if (CurrentInteractionData->bIsUsed)
	{UE_LOG(LogTemp, Warning, TEXT("3"));
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(
				HideTimerHandle,
				this,
				&UHideableComponent::Hide, 
				HideTickRate,                    
				true                             
			);
		
		}
		
	}
	else
	{
		PlayerComp->Hide();
		GetWorld()->GetTimerManager().SetTimer(
				HideTimerHandle,
				this,
				&UHideableComponent::Hide, 
				HideTickRate,                    
				true                             
			);
	}
}
void UHideableComponent::Hide()
{
	UE_LOG(LogTemp, Warning, TEXT("4"));
	FVector Target = CurrentInteractionData->bIsUsed ? QuitLocation : HiddenLocation;
	FVector Current = PlayerRef->GetActorLocation();
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	float Speed = 300.f;

	FVector NewPos = UKismetMathLibrary::VInterpTo_Constant(Current, Target, DeltaTime, Speed);

	FTransform NewTransform;
	NewTransform.SetLocation(NewPos);
	
	PlayerRef->SetActorTransform(NewTransform);

	float DistSqr = FVector::DistSquared(Target, PlayerRef->GetActorLocation());
	if (DistSqr < FMath::Square(0.01f))
	{
		GetWorld()->GetTimerManager().ClearTimer(HideTimerHandle);
		PlayerComp->QuitHiding();
	}
	
}
void UHideableComponent::Quit()
{
	UE_LOG(LogTemp, Warning, TEXT("5"));
	if (CurrentInteractionData->bIsUsed)
	{
		CurrentInteractionData->bIsUsed = false;
		PlayerComp->Hide();
	}
	else
	{
		CurrentInteractionData->bIsUsed = true;	
	}
}