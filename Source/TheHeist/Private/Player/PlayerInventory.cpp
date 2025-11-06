// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerInventory.h"
#include "Kismet/GameplayStatics.h"


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

void UPlayerInventory::AddItem(TSubclassOf<AGadgets> ItemClass)
{
	{
		for (FInventorySlot& Slot : Items)
		{
			if (Slot.ItemClass == ItemClass)
			{
				Slot.Quantity++;
				OnInventoryUpdated.Broadcast(Items);
				return;
			}
		}

		
		FInventorySlot NewSlot;
		NewSlot.ItemClass = ItemClass;
		NewSlot.Quantity = 1;
		Items.Add(NewSlot);

		OnInventoryUpdated.Broadcast(Items);
	}
}

void UPlayerInventory::StartUseItem()
{
	FInventorySlot& Slot = Items[CurrentItemIndex];
	TSubclassOf<AGadgets> ItemClass = Slot.ItemClass;

	if (!ItemClass)
		return;

	if (Slot.Quantity <= 0)
	{
		return;
	}
	if (FindOrCacheGadget(ItemClass))
	{
		RecallGadget(FindOrCacheGadget(ItemClass));
		UE_LOG(LogTemp, Warning, TEXT("%s is already spawned, recalling it by cash"), *ItemClass->GetName());
		if (!Slot.bIsStack)
		{
			Slot.Quantity--;
		}
		return;
	}

	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(GetOwner());

	AGadgets* SpawnedGadget = World->SpawnActor<AGadgets>(
		ItemClass,
		GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 100.f,
		GetOwner()->GetActorRotation(),
		SpawnParams
	);

	if (SpawnedGadget)
	{
		SpawnedGadgets.Add(ItemClass, SpawnedGadget);
		CurrentGadget = SpawnedGadget;
		CurrentGadget->OnUsePressed();
		CurrentGadget->ChangeCanBeUsed();
		CurrentGadget->CooldownTimer();

		Slot.Quantity--;

		UE_LOG(LogTemp, Warning, TEXT("%s used, remaining: %d"), *ItemClass->GetName(), Slot.Quantity);
	}
}

AGadgets* UPlayerInventory::FindOrCacheGadget(TSubclassOf<AGadgets> ItemClass)
{
	if (!ItemClass) return nullptr;

	if (CachedGadgets.Contains(ItemClass) && IsValid(CachedGadgets[ItemClass]))
	{
		CurrentGadget = CachedGadgets[ItemClass];
		return CurrentGadget;
	}

	UWorld* World = GetWorld();
	if (!World) return nullptr;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, ItemClass, FoundActors);

	for (AActor* Actor : FoundActors)
	{
		AGadgets* Gadget = Cast<AGadgets>(Actor);
		if (IsValid(Gadget))
		{
			CurrentGadget = Gadget;
			CachedGadgets.Add(ItemClass, Gadget);
			return CurrentGadget;
		}
	}

	return nullptr;
}


void UPlayerInventory::RecallGadget(AGadgets* Gadget)
{
	if (!Gadget || !GetOwner()) return;

	FVector SpawnLocation = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 100.f;
	Gadget->SetActorLocation(SpawnLocation);
	Gadget->SetActorRotation(GetOwner()->GetActorRotation());
	Gadget->OnUsePressed();
	Gadget->ChangeCanBeUsed();
	Gadget->CooldownTimer();
	

	UE_LOG(LogTemp, Warning, TEXT("%s recalled"), *Gadget->GetName());
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

			FTimerHandle& TimerHandle = CooldownTimers.FindOrAdd(UsedClass);
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle,
				[this, UsedClass]()
				{
					ActiveCooldowns.Remove(UsedClass);
					CooldownTimers.Remove(UsedClass); // cleanup
				},
				Cooldown,
				false
			);
		}
	}
	OnInventoryUpdated.Broadcast(Items);
}

void UPlayerInventory::ModifyCurrentIndex(int m_Value)
{
	CurrentItemIndex += m_Value;

	if (CurrentItemIndex >= Items.Num())
	{
		CurrentItemIndex = 0;
	}
	if (CurrentItemIndex < 0)
	{
		CurrentItemIndex = Items.Num() - 1;
	}

	OnInventoryUpdated.Broadcast(Items);
}


