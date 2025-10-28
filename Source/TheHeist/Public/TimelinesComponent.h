// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "TimelinesComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimelineProgress, float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimelineFinished);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEHEIST_API UTimelinesComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	//--------------Functions

	UTimelinesComponent();
	
	UFUNCTION(BlueprintCallable)
	void PlayTimeline(UCurveFloat* InCurve, float InDuration, bool bReverse = false);

	UFUNCTION(BlueprintCallable)
	bool IsPlaying() const { return Timeline.IsPlaying(); }
	
	//-------------Properties

	//Event called whenever the timeline has done playing
	UPROPERTY(BlueprintAssignable)
	FOnTimelineFinished OnTimelineFinished;
	
	UPROPERTY(BlueprintAssignable)
	FOnTimelineProgress OnTimelineProgress;
	
	
	

protected:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
private:
	
	//Calls the timeline progress with the alpha value
	UFUNCTION()
	void HandleTimelineProgress(float Value);

	UPROPERTY()
	FTimeline Timeline;

	bool bHasFinished = false;
		
};
