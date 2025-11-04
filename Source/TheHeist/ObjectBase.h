// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "ObjectDataAsset.h"
#include "ObjectInterface.h"
#include "GameFramework/Actor.h"
#include "ObjectBase.generated.h"

UCLASS()
class THEHEIST_API AObjectBase : public AActor, public  IObjectInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObjectBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PlayAnimation(UAnimMontage* Anim);

	void StopAnimation();

	void PlayNiagara(UNiagaraSystem* NiagaraSystem);

	void StopNiagara();

	void PlayAudio(USoundBase* Sound);

	void StopAudio();
	
	void ExistObject();

	virtual void UseObject_Implementation() override;

	void ReturnObject();

	
	UPROPERTY(EditAnywhere)
	UObjectDataAsset* ObjectDataAsset;

};
