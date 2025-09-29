// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gadgets.generated.h"

UCLASS()
class THEHEIST_API AGadgets : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGadgets();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gadgets")
	UStaticMeshComponent* StaticMesh;

	//Amount of this gadget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gadgets")
	int Amount;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gadgets")
	FString GadgetName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gadgets")
	int MaxAmount;

	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Function to use every gadget. Must be overrided.
	UFUNCTION(BlueprintCallable, Category = "Gadgets")
	virtual void UseGadget();

};
