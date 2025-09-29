#pragma once

#include "CoreMinimal.h"
#include "InteractionsEnum.generated.h"

UENUM(BlueprintType)
enum class EActivableType : uint8
{
	Light        UMETA(DisplayName = "Light"),
	Sound   UMETA(DisplayName = "Sound"),
};

UENUM(BlueprintType)
enum class EOpenableType : uint8
{
	Door       UMETA(DisplayName = "Rotate (door)"),
	Drawer  UMETA(DisplayName = "Translate (drawer)"),
};