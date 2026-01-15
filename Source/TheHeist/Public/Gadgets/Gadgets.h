// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Datas/DataAssets/Gadget.h"
#include "GameFramework/Character.h"
#include "Gadgets.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGadgetUsedDelegate, AGadgets*, Gadget);
DECLARE_DYNAMIC_DELEGATE(FSetDataAsset);
UCLASS()
class THEHEIST_API AGadgets : public AActor
{
	GENERATED_BODY()
	
public:

	//Event dispatcher, called when the gadget is used
	UPROPERTY(BlueprintAssignable, Category="Gadget")
	FGadgetUsedDelegate OnGadgetUsed;

	//Function to call the dispatcher
	UFUNCTION(BlueprintCallable, Category="Gadget")
	void CallOnGadgetUsed()
	{
		OnGadgetUsed.Broadcast(this);
	}
	
	// Sets default values for this actor's properties
	AGadgets();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gadgets")
	UStaticMeshComponent* StaticMesh;

	//Quantity of this gadget used
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gadgets")
	int Amount;

	//Cooldown for the gadget's use. Default is 0, meaning no cooldown
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gadgets")
	float CooldownDuration = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gadgets")
	FString GadgetName;

	//Max amount of stack of this object in the inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gadgets")
	int MaxAmount = 1;

	//True if the gadget quantity can be decreased
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gadgets")
	bool AmountCanBeDecreased = true;

	
protected:
	FTimerHandle TimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Cooldown = 5.0f;

	bool bCanBeUse = true;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category="Gadget")
	UGadget* DA_Gadget;

private:
	UPROPERTY()
	ACharacter* CharacterOwner;

	bool bIsTaking;

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Event to use every gadget. Called when button pressed. Must be overrided.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Gadget")
	void OnUsePressed();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Gadget")
	void OnDropPressed();
	
	//Event to use every gadget. Called when button released. Must be overrided.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Gadget")
	void OnUseReleased();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Gadget")
	void OnAttachGadget(USkeletalMeshComponent* SkeletalMesh);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Gadget")
	void OnDettachGadget();

	//returns the cooldown duration
	UFUNCTION(BlueprintCallable, Category="Gadget")
	float GetCooldown(){return CooldownDuration;}

	//returns the max quantity
	UFUNCTION(BlueprintCallable, Category="Gadget")
	float GetMaxAmount(){return MaxAmount;}

	void ChangeCanBeUsed()
	{
		bCanBeUse = !bCanBeUse;
	}
	void CooldownTimer();
	
	UFUNCTION(BlueprintPure, Category="Gadget")
	UGadget* GetDataAsset() {
		if (!DA_Gadget)
		{
			return nullptr;
		}
		return DA_Gadget;
	}

	void SetDataAsset(UGadget* NewGadget)
	{
		DA_Gadget = NewGadget;
	}

	void SetCharacter(ACharacter* Character)
	{
		CharacterOwner = Character;
	}
	float PlayMontage(UAnimMontage* Montage, float Rate = 1.0f)
	{
		return CharacterOwner->GetMesh()->GetAnimInstance()->Montage_Play(Montage, Rate);;
	}
	
	float TakeGadget();


	void SetNoPhysicObject();

	bool GetIsTaking()
	{
		return bIsTaking;
	}

	UAnimMontage* GetAnimation()
	{
		if (DA_Gadget)
		{
			if (DA_Gadget->GadgetStruct.EffectGadget.AnimationGadgetUse)
			{
				return DA_Gadget->GadgetStruct.EffectGadget.AnimationGadgetUse;
			}
			return nullptr;
		}
		return nullptr;
	}
};
