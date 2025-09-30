
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Gadgets/Gadgets.h"
#include "PlayerInventory.generated.h"

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AGadgets> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity = 1;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEHEIST_API UPlayerInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerInventory();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//keeps track of object's cooldowns
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	TMap<TSubclassOf<AGadgets>, float> ActiveCooldowns;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Inventory")
	TArray<FInventorySlot> Items;
	
	UFUNCTION(BlueprintCallable)
	void AddItem(TSubclassOf<AGadgets> ItemClass)
	
	{
		for (FInventorySlot& Slot : Items)
		{
			if (Slot.ItemClass == ItemClass)
			{
				Slot.Quantity++;
				return;
			}
		}

		
		FInventorySlot NewSlot;
		NewSlot.ItemClass = ItemClass;
		NewSlot.Quantity = 1;
		Items.Add(NewSlot);
	}
	UFUNCTION(BlueprintCallable)
	void StartUseItem(TSubclassOf<AGadgets> ItemClass);
	UFUNCTION(BlueprintCallable)
	void RelaseUseItem();
	
	AGadgets* CurrentGadget;

	UFUNCTION(BlueprintCallable)
	void OnGadgetUsed(AGadgets* GadgetUsed);
};
