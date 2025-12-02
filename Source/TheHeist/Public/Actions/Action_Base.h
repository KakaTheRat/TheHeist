// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Datas/Enumerators/Actions/ActionsEnum.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/NoExportTypes.h"
#include "Action_Base.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActionEnded);

UCLASS(Blueprintable)
class THEHEIST_API UAction_Base : public UObject
{
	GENERATED_BODY()

	public:

	//Properties//
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Actions", meta=(AllowPrivateAccess=true))
	FName ActionName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Actions", meta=(AllowPrivateAccess=true))
	float Cooldown;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Actions", meta=(AllowPrivateAccess=true))
	bool bRequiresTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Actions", meta=(AllowPrivateAccess=true))
	EActionsTargetTypes TargetType = EActionsTargetTypes::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Actions", meta=(AllowPrivateAccess=true))
	UAnimMontage* AnimationMontage;
	
	UPROPERTY(BlueprintAssignable, Category="Actions")
	FOnActionEnded OnActionEndedDelegate;
	
	//Functions

	
	
	UFUNCTION(BlueprintCallable, Category="Actions")
	bool CanExecute();

	UFUNCTION(BlueprintCallable, Category="Actions")
	void OnActionEnded();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Actions")
	void OnExecute(AActor* OwningGuard);
	virtual void OnExecute_Implementation(AActor* OwningGuard);

	UFUNCTION(BlueprintCallable, Category="Actions")
	void OnActionCompleted();
};
