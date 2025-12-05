// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactions/InteractableComponent.h"
#include "ActionActor.generated.h"

UCLASS()
class THEHEIST_API AActionActor : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AActionActor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ActionActor")
	UInteractableComponent* InteractableComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ActionActor")
	TArray<USceneComponent*> InteractionPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SmartObject")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere,Category="SmartObject")
	TArray<AActor*> RegisteredActors;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SmartObject")
	int MaxUsers;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category="SmartObject")
	bool IsAvailable() const;

	UFUNCTION(BlueprintCallable, Category="SmartObject")
	bool RegisterUser(AActor* User);

	UFUNCTION(BlueprintCallable, Category="SmartObject")
	void UnregisterUser(AActor* User);

};
