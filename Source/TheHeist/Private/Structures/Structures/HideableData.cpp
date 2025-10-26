#include "Structures/Interactions/HideableData.h"


UHideableData::UHideableData()
{
	InteractText = "Hide";
}

void UHideableData::ExecuteInteraction(AActor* Owner, USceneComponent* Target, EInteractionContext Context, AActor* InteractingActor)
{
	Super::ExecuteInteraction(Owner, Target, Context, nullptr);
	
	if (Context == EInteractionContext::Guard)
	{
		if (HiddenActor != nullptr)
		{
			// Si quelqu’un est caché → le faire sortir
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("🚨 Guard found a hidden pawn! Forcing them out..."));

			HiddenActor->SetActorLocation(QuitLocation);
			HiddenActor = nullptr;
		}
		else
		{
			// Sinon, le garde peut se cacher lui-même
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("👮 nothing to be found..."));
		}
		return;
	}
	
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
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "HIDeD");
			PlayerComp->QuitHiding();
			HiddenActor = nullptr;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "NOT HIDeD");
			HiddenActor = InteractingActor;
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
		GEngine->AddOnScreenDebugMessage(
	-1,
	15.0f,
	FColor::Red,
	FString::Printf(TEXT("bIsUsed: %s"), bIsUsed ? TEXT("true") : TEXT("false"))
);
		EndOfInteraction();

	}
}