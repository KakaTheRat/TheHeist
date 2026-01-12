// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/Action_Base.h"
#include "Talk_Action.generated.h"

/**
 * 
 */
UCLASS()
class THEHEIST_API UTalk_Action : public UAction_Base
{
	GENERATED_BODY()

public :

	//Functions//
	
	virtual void OnExecute_Implementation(AActor* OwningGuard) override;
	
};
