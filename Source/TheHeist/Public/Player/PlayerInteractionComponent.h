// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Widget/Interaction/InteractionWidgetActor.h"
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

	// Event to handle the hiding state of the player
	UFUNCTION(BlueprintImplementableEvent)
	void Hide();

	// Event to handle the unhiding state of the player
	UFUNCTION(BlueprintImplementableEvent)
	void QuitHiding();

	UFUNCTION()
	AInteractionWidgetActor* GetInteractionWidget(){return WidgetInstance;};
	

private:

	//Interaction widget class
	UPROPERTY(EditAnywhere,Category="Interaction")
	TSubclassOf<AInteractionWidgetActor> WidgetActorClass;

	//Widget object's instance
	UPROPERTY()
	AInteractionWidgetActor* WidgetInstance;
};
