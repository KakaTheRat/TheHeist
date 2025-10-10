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

	//------------Functions--------------//

	virtual void Tick(float DeltaTime) override;
	
	// Sets default values for this actor's properties
	AInteractionWidgetActor();

	// Returns widget. Widget's Getter
	UUserWidget* GetWidget() const { return WidgetComp ? WidgetComp->GetWidget() : nullptr; }

	//Adds a new interaction button to the widget whenever is called. Is expecting an interaction text
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void AddInteractionEntry(const FString& Entry);

	

	//----------------Properties--------------//
	
	// Widget Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* WidgetComp;

	//Reference to the attached widget
	UPROPERTY(BlueprintReadWrite, Category="Interaction")
	UInteractionWindowWidget* MyWidget;
	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
