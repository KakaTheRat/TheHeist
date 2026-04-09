#pragma once

#include "CoreMinimal.h"
#include "Interactions/InteractionTypes/InteractionData.h"
#include "MonitorFocusData.generated.h"

UCLASS()
class THEHEIST_API UMonitorFocusData : public UInteractionData
{
	GENERATED_BODY()

public:
	
	UMonitorFocusData();
	
	virtual void StartInteraction() override;
};