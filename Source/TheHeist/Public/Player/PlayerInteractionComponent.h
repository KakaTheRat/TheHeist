// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerInteractionComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEHEIST_API UPlayerInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Event que le BP peut implémenter pour gérer l'entrée dans la cachette
	UFUNCTION(BlueprintImplementableEvent, Category="Hiding")
	void Hide();

	// Event que le BP peut implémenter pour gérer la sortie de la cachette
	UFUNCTION(BlueprintImplementableEvent, Category="Hiding")
	void QuitHiding();

		
};
