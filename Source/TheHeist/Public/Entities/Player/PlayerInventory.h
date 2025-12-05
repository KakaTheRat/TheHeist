
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Gadgets/Gadgets.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUpdateWidget, bool, State, int32, Index);

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

private:
	UPROPERTY(EditAnywhere)
	TArray<UGadget*> AllDataAssets;

	int32 CurrentGadgetIndex;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	TArray<FInventorySlot> Items;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	TArray<TSubclassOf<AGadgets>> InventoryGadgets;

	UPROPERTY()
	ACharacter* Character;

	UPROPERTY()
	AGadgets* CurrentGadget;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<AGadgets*> HardRefGadgets;

	UPROPERTY()
	APawn* Pawn ;
	
	UPROPERTY()
	TMap<TSubclassOf<AGadgets>, AGadgets*> SpawnedGadgets;

	UPROPERTY()
	TMap<TSubclassOf<AGadgets>, AGadgets*> CachedGadgets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputMappingContext* InputMapping;
	
	UFUNCTION(BlueprintCallable)
	void AddItem(TSubclassOf<AGadgets> ItemClass);

	//Called when gadget's use input pressed
	//UFUNCTION(BlueprintCallable)
	//void StartUseItem();

	//Called when gadget's use input released
	UFUNCTION(BlueprintCallable)
	void RelaseUseItem();

	AGadgets* FindActor(TSubclassOf<AGadgets> ItemClass);

	void RecallGadget(AGadgets* Gadget);
	
	//Called whenever a gadget is used. Commonly called after the gadget's use dispatcher
	UFUNCTION(BlueprintCallable)
	void OnGadgetUsed(AGadgets* GadgetUsed);

	UFUNCTION(BlueprintCallable)
	void ModifyCurrentIndex(int m_Value);

	AGadgets* SpawnAndCacheGadget(TSubclassOf<AGadgets> ItemClass);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* USeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* GadgetOne;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* GadgetTwo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* GadgetThree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* GadgetFor;

	void Use(const FInputActionValue& Value);
	
	void InputOne(const FInputActionValue& Value);
	void InputTwo(const FInputActionValue& Value);
	void InputThree(const FInputActionValue& Value);
	void InputFore(const FInputActionValue& Value);

	void ChangeCurrentGadget(int32 Value);

	int32 PressedInt;

	UGadget* FindDataAssets(FString Name);

	FString CleanName(const FString& InputName);

	UPROPERTY(EditAnywhere, BlueprintAssignable)
	FUpdateWidget UpdateWidget;
};
