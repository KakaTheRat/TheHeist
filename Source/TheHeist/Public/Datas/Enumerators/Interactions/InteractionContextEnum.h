#pragma once

#include "CoreMinimal.h"
#include "InteractionContextEnum.generated.h"

UENUM(BlueprintType)
enum class EInteractionContext : uint8
{
	Default UMETA(DisplayName = "Default"),
	Guard UMETA(DisplayName = "Guard")
};