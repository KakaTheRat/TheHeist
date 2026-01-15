// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Gadget.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FGeneralGadget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category = "General")
	FName Name = FName("");

	UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category = "General")
	FString Description = FString("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	UTexture2D* Icon = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	UMaterialInterface* IconMaterial = nullptr;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	UStaticMesh* Mesh = nullptr;

};

USTRUCT(BlueprintType)
struct FDetailGadget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detail")
	bool bIsProjectile = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detail")
	bool bIsDrone = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detail")
	bool bIsSmoke = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detail", meta = (EditCondition = "bIsProjectile", EditConditionHides))
	float ProjectileSpeedXY = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detail", meta = (EditCondition = "bIsProjectile", EditConditionHides))
	float ProjectileSpeedz = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detail", meta = (EditCondition = "bIsDrone", EditConditionHides))
	float SpeedDrone = 10000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detail", meta = (EditCondition = "bIsDrone", EditConditionHides))
	float JumpDrone = 30000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detail", meta = (EditCondition = "bIsDrone", EditConditionHides))
	float VelocityAngular = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detail", meta = (EditCondition = "bIsDrone",EditConditionHides))
	float MinFildOfView = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detail", meta = (EditCondition = "bIsDrone", EditConditionHides))
	float MaxFildOfView = 90.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detail", meta = (EditCondition = "bIsSmoke", EditConditionHides))
	float MAxStack = 0;
};


USTRUCT(BlueprintType)
struct FEffectGadget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	USoundBase* ExitSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	USoundBase* ReturnSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UNiagaraSystem* NiagaraEffect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UAnimMontage* AnimationGadget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UAnimMontage* AnimationGadgetUse = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool CanDrop = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (EditCondition = "CanDrop", EditConditionHides))
	class UAnimMontage* AnimationGadgetDrop = nullptr;
};

USTRUCT(BlueprintType)
struct FGadgetStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	FGeneralGadget GeneralGadget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detail")
	FDetailGadget DetailGadget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FEffectGadget EffectGadget;
};
UCLASS( BlueprintType)
class THEHEIST_API UGadget : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGadgetStruct GadgetStruct;
};
