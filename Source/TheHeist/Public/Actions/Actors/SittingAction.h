// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionActor.h"
#include "SittingAction.generated.h"

UCLASS()
class THEHEIST_API ASittingAction : public AActionActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASittingAction();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere)
	UAnimMontage* Montage;
	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
