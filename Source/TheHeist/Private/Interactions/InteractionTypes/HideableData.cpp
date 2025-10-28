#include "Interactions/InteractionTypes/HideableData.h"

UHideableData::UHideableData()
{
	InteractText = "Hide";
}

void UHideableData::ExecuteInteraction(AActor* Owner, USceneComponent* Target, EInteractionContext Context, AActor* InteractingActor)
{
	Super::ExecuteInteraction(Owner, Target, Context, nullptr);
	CurrentInteractingActor = InteractingActor;

	if (!Owner)
		return;

	// ===  GUARD CASE CONTEXT ===
	if (Context == EInteractionContext::Guard)
	{
		if (HiddenActor)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT(" Guard found a hidden pawn! Forcing them out..."));
			HiddenActor->SetActorLocation(QuitLocation);
			
			if (AActor* FoundActor = HiddenActor)
			{
				if (UPlayerInteractionComponent* PlayerCompp = FoundActor->FindComponentByClass<UPlayerInteractionComponent>())
				{
					PlayerCompp->QuitHiding();
				}
			}//MUST USE AN INTERFACE Or ANYTHING ELSE !!! (Later)

			HiddenActor = nullptr;
			EndOfInteraction();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT(" Nothing to be found..."));
			EndOfInteraction();
		}
		return;
	}

	// === DEFAULT CONTEXT  ===
	APlayerController* PC = Owner->GetWorld()->GetFirstPlayerController();
	PlayerRef = PC ? PC->GetPawn() : nullptr;
	if (!PlayerRef.IsValid())
		return;

	PlayerComp = PlayerRef->FindComponentByClass<UPlayerInteractionComponent>();

	
	TArray<USceneComponent*> Components;
	Owner->GetComponents<USceneComponent>(Components);
	for (USceneComponent* Comp : Components)
	{
		if (!Comp) continue;

		if (Comp->GetName() == HiddenArrowName)
			HiddenLocation = Comp->GetComponentLocation();
		else if (Comp->GetName() == QuitArrowName)
			QuitLocation = Comp->GetComponentLocation();
	}

	if (PlayerComp.IsValid())
	{
		if (bIsUsed)
		{
	
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Start exiting hideout...");
			HiddenActor = nullptr;
		}
		else
		{
		
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Hiding...");
			HiddenActor = InteractingActor;
			PlayerComp->Hide(); 
		}
	}

	// === Movement launch ===
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
	if (!Owner || !PlayerRef.IsValid())
		return;

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
			FColor::Green,
			FString::Printf(TEXT("bIsUsed: %s"), bIsUsed ? TEXT("true") : TEXT("false"))
		);
		
		if (bIsUsed == false && PlayerComp.IsValid())
		{
			PlayerComp->QuitHiding();
		}

		EndOfInteraction();
	}
}
