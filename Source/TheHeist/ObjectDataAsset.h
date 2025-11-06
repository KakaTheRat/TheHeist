// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NiagaraFunctionLibrary.h"
#include "ObjectDataAsset.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FObjectGenral
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "General")
	FName Name = "";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "General")
	FString Description = "";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "General")
	UStaticMesh* Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "General")
	float Cooldown = 0.0f;
};

USTRUCT(BlueprintType)
struct FObjectEffect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	USoundBase* ExitObjectSound = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	USoundBase* UsingObjectSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	USoundBase* ReturnObjectSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	UNiagaraSystem* ExitObjectNiagara = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	UNiagaraSystem* UsingObjectNiagara = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	UNiagaraSystem* ReturnObjectNiagara = nullptr;
	
	
};

USTRUCT(BlueprintType)
struct FObjectAnimation
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* ExistObjectAnimation = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	UAnimMontage* UsingObjectAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	UAnimMontage* ReturnObjectAnimation = nullptr;
};

USTRUCT(BlueprintType)
struct FObjectStrut
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "General")
	FObjectGenral General;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	FObjectEffect Effect;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	FObjectAnimation Animation;
	
};

UCLASS()
class THEHEIST_API UObjectDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	FObjectStrut ObjectStruct;
	
};
