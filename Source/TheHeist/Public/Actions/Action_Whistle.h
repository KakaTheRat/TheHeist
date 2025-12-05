// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action_Base.h"
#include "Action_Whistle.generated.h"


UCLASS()
class THEHEIST_API UAction_Whistle : public UAction_Base
{
	GENERATED_BODY()

public :
	
	
	//Functions//
	
	virtual void OnExecute_Implementation(AActor* OwningGuard) override;


	//Properties//
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Actions", meta=(AllowPrivateAccess=true))
	USoundBase* f;
};
