
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsStack = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryUpdated, const TArray<FInventorySlot>&, Items);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEHEIST_API UPlayerInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerInventory();

	//Event dispatcher, each time a gadget slot is updated
	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnInventoryUpdated OnInventoryUpdated;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//keeps track of object's cooldowns
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	TMap<TSubclassOf<AGadgets>, float> ActiveCooldowns;

	TMap<TSubclassOf<AGadgets>, FTimerHandle> CooldownTimers;

	//Index to determine the item currently being used
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	int CurrentItemIndex = 0;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	TArray<FInventorySlot> Items;

	UPROPERTY()
	TMap<TSubclassOf<AGadgets>, AGadgets*> SpawnedGadgets;

	UPROPERTY()
	TMap<TSubclassOf<AGadgets>, AGadgets*> CachedGadgets;
	
	UFUNCTION(BlueprintCallable)
	void AddItem(TSubclassOf<AGadgets> ItemClass);

	//Called when gadget's use input pressed
	UFUNCTION(BlueprintCallable)
	void StartUseItem();

	//Called when gadget's use input released
	UFUNCTION(BlueprintCallable)
	void RelaseUseItem();

	AGadgets* FindOrCacheGadget(TSubclassOf<AGadgets> ItemClass);

	void RecallGadget(AGadgets* Gadget);

	
	
	AGadgets* CurrentGadget;

	//Called whenever a gadget is used. Commonly called after the gadget's use dispatcher
	UFUNCTION(BlueprintCallable)
	void OnGadgetUsed(AGadgets* GadgetUsed);

	UFUNCTION(BlueprintCallable)
	void ModifyCurrentIndex(int m_Value);
};
