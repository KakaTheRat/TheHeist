// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionInterface.h"
#include "Components/ActorComponent.h"
#include "InteractableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEHEIST_API UInteractableComponent : public UActorComponent, public IInteractionInterface
{
	GENERATED_BODY()

public:

	// Amount of attached components.
	UPROPERTY(EditAnywhere, Category="Interactable")
	int32 AmountComponents = 0;

	// List of names for every component attached (depending on the NumComponent)
	UPROPERTY(EditAnywhere, Category="Interactable")
	TArray<FString> NamesOfComponents;

	//Text printed when interacted with
	UPROPERTY(EditAnywhere, Category="Interactable")
	FString InteractText;
	
	// Sets default values for this component's properties
	UInteractableComponent();

protected:

	//**Variables**//


	//Scene component which is accessing to this component
	UPROPERTY(Blueprintable, Category=Interactable, EditAnywhere)
	USceneComponent* AttachedComponent;


	
	
	
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Interact override function
	virtual void Interact_Implementation(USceneComponent* HitComponent) override;

	//Finds the attached component
	void FindAttachedComponent();
	
#if WITH_EDITOR
	// Called when a property is modified within the editor
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
		
};
