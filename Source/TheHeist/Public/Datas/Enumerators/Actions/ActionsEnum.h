#pragma once

#include "CoreMinimal.h"
#include "ActionsEnum.generated.h"

UENUM(BlueprintType)
enum class EActionsTargetTypes : uint8
{
	None,
	Position,
	Actor,
	SmartObject,
};