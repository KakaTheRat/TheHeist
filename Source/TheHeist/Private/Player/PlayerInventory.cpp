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
	Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	ACharacter* Char = Cast<ACharacter>(Pawn);
	for (auto Gadget : InventoryGadgets)
	{
		AGadgets* Tmp = FindActor(Gadget);
		if (!Tmp)
		{
			continue;
		}
		Tmp->SetCharacter(Char);
		HardRefGadgets.Add(Tmp);
	}
	Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	
	APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	if (!PC) return;

	if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
        
		if (Subsystem && InputMapping)
		{
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}

	
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PC->InputComponent))
	{
		if (USeAction)
		{
			EnhancedInput->BindAction(USeAction, ETriggerEvent::Started, this, &UPlayerInventory::Use);
			EnhancedInput->BindAction(USeAction, ETriggerEvent::Completed, this, &UPlayerInventory::RelaseUseItem);
		}
		if (GadgetOne)
		{
			EnhancedInput->BindAction(GadgetOne, ETriggerEvent::Triggered, this, &UPlayerInventory::InputOne);
		}
		if (GadgetTwo)
		{
			EnhancedInput->BindAction(GadgetTwo, ETriggerEvent::Triggered, this, &UPlayerInventory::InputTwo);
		}
		if (GadgetThree)
		{
			EnhancedInput->BindAction(GadgetThree, ETriggerEvent::Triggered, this, &UPlayerInventory::InputThree);
		}
		if (GadgetFor)
		{
			EnhancedInput->BindAction(GadgetFor, ETriggerEvent::Triggered, this, &UPlayerInventory::InputFore);
		}
	}
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
	if (FindActor(ItemClass))
	{
		RecallGadget(FindActor(ItemClass));
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

AGadgets* UPlayerInventory::FindActor(TSubclassOf<AGadgets> ItemClass)
{
	if (!*ItemClass)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, ItemClass, FoundActors);

	if (FoundActors.Num() > 0)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("trouver"));
		}
		AGadgets* Tmp = Cast<AGadgets>(FoundActors[0]);
		if (!Tmp->GetDataAsset())
		{
			Tmp->SetDataAsset(FindDataAssets(Tmp->GetName()));
			Tmp->DelegateDataAsset.ExecuteIfBound();
		}
		return Cast<AGadgets>( FoundActors[0]);
	}

	return SpawnAndCacheGadget(ItemClass);
}


AGadgets* UPlayerInventory::SpawnAndCacheGadget(TSubclassOf<AGadgets> ItemClass)
{
	if (!ItemClass) return nullptr;

	UWorld* World = GetWorld();
	if (!World) return nullptr;

	FActorSpawnParameters Params;
	Params.Owner = GetOwner();
	Params.Instigator = Cast<APawn>(GetOwner());

	FVector SpawnLocation = FVector(0.f, 0.f, 0.f);
	FRotator SpawnRotation = GetOwner()->GetActorRotation();

	AGadgets* Gadget = World->SpawnActor<AGadgets>(ItemClass, SpawnLocation, SpawnRotation, Params);
	if (!IsValid(Gadget)) return nullptr;

	CachedGadgets.Add(ItemClass, Gadget);

	Gadget->SetCharacter(Character);
	Gadget->SetDataAsset(FindDataAssets(Gadget->GetName()));
	return Gadget;
}



void UPlayerInventory::RecallGadget(AGadgets* Gadget)
{
	if (!Gadget || !GetOwner()) return;

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

void UPlayerInventory::Use(const FInputActionValue& Value)
{
	if (!CurrentGadget)
	{
		return;
	}
	RecallGadget(CurrentGadget);
}

void UPlayerInventory::InputOne(const FInputActionValue& Value)
{
	ChangeCurrentGadget(0);
}

void UPlayerInventory::InputTwo(const FInputActionValue& Value)
{
	ChangeCurrentGadget(1);
}

void UPlayerInventory::InputThree(const FInputActionValue& Value)
{
	ChangeCurrentGadget(2);
}

void UPlayerInventory::InputFore(const FInputActionValue& Value)
{
	ChangeCurrentGadget(3);
}

void UPlayerInventory::ChangeCurrentGadget(int32 Value)
{
	if (HardRefGadgets.IsValidIndex(Value))
	{
		CurrentGadget = HardRefGadgets[Value];

		FVector SpawnLocation = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 100.f;

		CurrentGadget->SetActorLocation(SpawnLocation);
		
		CurrentGadget->SetActorRotation(GetOwner()->GetActorRotation());
		CurrentGadget->SetNoPhysicObject();
		CurrentGadget->TakeGadget();
		
	}
}

UGadget* UPlayerInventory::FindDataAssets(FString Name)
{
	if (AllDataAssets.IsEmpty())
	{
		return nullptr;
	}
	for (auto Data : AllDataAssets)
	{
		if (Data->GetName().Contains(CleanName(Name)))
		{
			return Data;
		}
	}
	return nullptr;
}

FString UPlayerInventory::CleanName(const FString& InputName)
{
	FString Result = InputName;

	if (Result.StartsWith(TEXT("BP_")))
	{
		Result = Result.RightChop(3);
	}
	int32 Index;
	if (Result.FindChar('_', Index))
	{
		if (Result.Mid(Index, 3) == "_C_")
		{
			Result = Result.Left(Index);
		}
	}

	return Result;
}

