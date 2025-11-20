#include "Interactions/InteractionTypes/CollectableData.h"

#include "Entities/EntitiesInterface.h"
#include "Entities/Player/PlayerInventory.h"

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
 		USkeletalMeshComponent* Mesh = IEntitiesInterface::Execute_GetSkeletalMeshComponent(InteractingActor);

 		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red,
	FString::Printf(TEXT("Mesh = %s"), *Mesh->GetName()));
 		if (!Mesh->DoesSocketExist("HandGrip_R"))
 		{
 			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red,
				 TEXT("Socket HandGrip_R DOES NOT EXIST on mesh !!"));

 			return;
 		}
 		Owner->SetActorEnableCollision(false);
 		Owner->AttachToComponent(
	Mesh,
	FAttachmentTransformRules::SnapToTargetIncludingScale,
	FName("HandGrip_R")
);
 		Inventory->AddItem(GadgetClass);
 		
 	}
 	else
 	{
 		UE_LOG(LogTemp, Warning, TEXT("No Inventory found on %s"), *PlayerRef->GetName());
 	}

	EndOfInteraction();
 }
