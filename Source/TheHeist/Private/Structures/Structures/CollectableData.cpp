#include "Structures/Interactions/CollectableData.h"

UCollectableData::UCollectableData()
{
	InteractText = "Collect";
}

void UCollectableData::ExecuteInteraction(AActor* Owner, USceneComponent* Target, EInteractionContext Context, AActor* InteractingActor)
 {
 	Super::ExecuteInteraction(Owner, Target, Context, nullptr);
 
 	if (!Owner) return;

 	APlayerController* PC = Owner->GetWorld()->GetFirstPlayerController();
 	APawn* PlayerRef = PC ? PC->GetPawn() : nullptr;
 	if (!PlayerRef) return;

 	UPlayerInventory* Inventory = PlayerRef->FindComponentByClass<UPlayerInventory>();
 	if (Inventory)
 	{
 		Inventory->AddItem(GadgetClass);
 		Owner->Destroy();
 	}
 	else
 	{
 		UE_LOG(LogTemp, Warning, TEXT("No Inventory found on %s"), *PlayerRef->GetName());
 	}

	EndOfInteraction();
 }
