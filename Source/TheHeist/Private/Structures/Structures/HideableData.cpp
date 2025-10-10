#include "Structures/Interactions/HideableData.h"


UHideableData::UHideableData()
{
	InteractText = "Hide";
}

void UHideableData::ExecuteInteraction(AActor* Owner, USceneComponent* Target)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "OKAY");
	Super::ExecuteInteraction(Owner, Target);
	
	if (!Owner) return;

	APlayerController* PC = Owner->GetWorld()->GetFirstPlayerController();
	PlayerRef = PC ? PC->GetPawn() : nullptr;
	if (!PlayerRef.IsValid()) return;
	
	PlayerComp = PlayerRef->FindComponentByClass<UPlayerInteractionComponent>();
	
	TArray<USceneComponent*> Components;
	Owner->GetComponents<USceneComponent>(Components);
	for (USceneComponent* Comp : Components)
	{
		if (!Comp) continue;

		if (Comp->GetName() == HiddenArrowName)
		{
			HiddenLocation = Comp->GetComponentLocation();
		}
		else if (Comp->GetName() == QuitArrowName)
		{
			QuitLocation = Comp->GetComponentLocation();
		}
	}
	if (PlayerComp.IsValid())
	{;
		if (bIsUsed)
		{
			PlayerComp->QuitHiding();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Chelou");
			PlayerComp->Hide();
				
		}
				
	}
	// Launch timer
	if (UWorld* World = Owner->GetWorld())
	{
		World->GetTimerManager().SetTimer(
			HideTimerHandle,
			[this, Owner]() { HideStep(Owner); },
			HideTickRate,
			true
		);
	}
	
}

void UHideableData::HideStep(AActor* Owner)
{
	if (!Owner || !PlayerRef.IsValid()) return;

	FVector Target = bIsUsed ? QuitLocation : HiddenLocation;
	FVector Current = PlayerRef->GetActorLocation();
	float DeltaTime = Owner->GetWorld()->GetDeltaSeconds();
	float Speed = 300.f;

	FVector NewPos = UKismetMathLibrary::VInterpTo_Constant(Current, Target, DeltaTime, Speed);
	PlayerRef->SetActorLocation(NewPos);
	if (FVector::DistSquared(Target, PlayerRef->GetActorLocation()) < FMath::Square(5.f))
	{
		Owner->GetWorld()->GetTimerManager().ClearTimer(HideTimerHandle);
		

		bIsUsed = !bIsUsed;
		OnInteractionEnded.ExecuteIfBound();
	}
}