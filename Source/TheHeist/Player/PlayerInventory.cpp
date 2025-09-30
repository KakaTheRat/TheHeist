// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerInventory.h"

// Sets default values for this component's properties
UPlayerInventory::UPlayerInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayerInventory::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPlayerInventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlayerInventory::StartUseItem(TSubclassOf<AGadgets> ItemClass)
{
	if (ActiveCooldowns.Contains(ItemClass))
	{
		float Remaining = ActiveCooldowns[ItemClass];
		if (Remaining > 0.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Item %s is on cooldown: %.2f sec left"), *ItemClass->GetName(), Remaining);
			return;
		}
	}
	
	UWorld* World = GetWorld();
	if (!World) return;

	for (FInventorySlot& Slot : Items)
	{
		if (Slot.ItemClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = Cast<APawn>(GetOwner());

			// Spawn l'Actor à la position du joueur
			AActor* SpawnedActor = World->SpawnActor<AActor>(
				Slot.ItemClass, 
				GetOwner()->GetActorLocation(), 
				GetOwner()->GetActorRotation(), 
				SpawnParams
			);

			if (SpawnedActor)
			{
				
				CurrentGadget = Cast<AGadgets>(SpawnedActor);
				if (CurrentGadget)
				{
					CurrentGadget->OnGadgetUsed.AddDynamic(this, &UPlayerInventory::OnGadgetUsed);
					CurrentGadget->OnUsePressed();
				}
				
			}
		}
	}
}

void UPlayerInventory::RelaseUseItem()
{
	if (CurrentGadget)
	{
		CurrentGadget->OnUseReleased();
	}
}

void UPlayerInventory::OnGadgetUsed(AGadgets* GadgetUsed)
{
	if (!GadgetUsed)return;
	{
	TSubclassOf<AGadgets> UsedClass = GadgetUsed->GetClass();
		
		for (FInventorySlot& Slot : Items)
		{
			if (Slot.ItemClass == UsedClass)
			{
				//Diminish quantity of this item
				if (GadgetUsed->AmountCanBeDecreased)
				{
					Slot.Quantity--;
					if (Slot.Quantity <= 0)
					{
						Slot.Quantity = 0;	
					}
					break;
				}
				
			}
				
		}

		//Starts the cooldown between 2 uses of the gadget
		float Cooldown = GadgetUsed->GetCooldown();
		if (Cooldown > 0.f)
		{
			ActiveCooldowns.Add(UsedClass, Cooldown);
			
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle,
				[this, UsedClass]()
				{
					ActiveCooldowns.Remove(UsedClass);
				},
				Cooldown,
				false
			);
		}
	}
}


