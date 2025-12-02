// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Action_Base.h"

bool UAction_Base::CanExecute()
{
	return true;
}

void UAction_Base::OnActionEnded()
{
	OnActionEndedDelegate.Broadcast();
}


void UAction_Base::OnExecute_Implementation(AActor* OwningGuard)
{
}

void UAction_Base::OnActionCompleted()
{
}
