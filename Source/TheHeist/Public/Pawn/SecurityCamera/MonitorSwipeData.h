#pragma once

#include "CoreMinimal.h"
#include "Interactions/InteractionTypes/InteractionData.h"
#include "MonitorSwipeData.generated.h"

UCLASS()
class THEHEIST_API UMonitorSwipeData : public UInteractionData
{
	GENERATED_BODY()

public:
	
	UMonitorSwipeData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swipe")
	bool bSwipeRight = true;

	virtual void StartInteraction() override;
};