// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionWindowWidget.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "InteractionWidgetActor.generated.h"

UCLASS()
class THEHEIST_API AInteractionWidgetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractionWidgetActor();

	// Returns widget
	UUserWidget* GetWidget() const { return WidgetComp ? WidgetComp->GetWidget() : nullptr; }

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable, Category="Interaction")
	void AddInteractionEntry(const FString& Entry);

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;
	
	// Widget Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* WidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	UInteractionWindowWidget* MyWidget;

};
