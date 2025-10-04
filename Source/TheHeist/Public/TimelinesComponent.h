// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "TimelinesComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimelineProgress, float, Value);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEHEIST_API UTimelinesComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTimelinesComponent();

	UPROPERTY(BlueprintAssignable)
	FOnTimelineProgress OnTimelineProgress;

	UFUNCTION(BlueprintCallable)
	void PlayTimeline(UCurveFloat* InCurve, float InDuration, bool bReverse = false);

	UFUNCTION(BlueprintCallable)
	bool IsPlaying() const { return Timeline.IsPlaying(); }

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	FTimeline Timeline;

	UFUNCTION()
	void HandleTimelineProgress(float Value);
		
};
