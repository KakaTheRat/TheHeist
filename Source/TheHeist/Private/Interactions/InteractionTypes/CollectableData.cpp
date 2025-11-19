#include "Interactions/InteractionTypes/CollectableData.h"

UCollectableData::UCollectableData()
{
	InteractText = "Collect";
}

void UCollectableData::StartInteraction()
{
	
 	APlayerController* PC = Owner->GetWorld()->GetFirstPlayerController();
 	APawn* PlayerRef = PC ? PC->GetPawn() : nullptr;
 	if (!PlayerRef) return;

 	UPlayerInventory* Inventory = PlayerRef->FindComponentByClass<UPlayerInventory>();
 	if (Inventory)
 	{
 		USkeletalMeshComponent* Mesh = InteractingActor->FindComponentByClass<USkeletalMeshComponent>();
 		Owner->SetActorEnableCollision(false);
 		Owner->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, "HandGrip_R");
 		Inventory->AddItem(GadgetClass);
 		
 	}
 	else
 	{
 		UE_LOG(LogTemp, Warning, TEXT("No Inventory found on %s"), *PlayerRef->GetName());
 	}

	EndOfInteraction();
 }
