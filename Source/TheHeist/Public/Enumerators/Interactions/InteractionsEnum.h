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
enum class EOpeningType : uint8
{
	Door       UMETA(DisplayName = "Rotate (door)"),
	Drawer  UMETA(DisplayName = "Translate (drawer)"),
};

UENUM(BlueprintType)
enum class EOpeningSide : uint8
{
	Right UMETA(DisplayName = "Right"),
	Left UMETA(DisplayName = "Left"),
	Up UMETA(DisplayName = "Up"),
	Down UMETA(DisplayName = "Down"),
	
};
UENUM(BlueprintType)
enum class EOpeningDirection : uint8
{
	Push UMETA(DisplayName = "Push"),
	Pull UMETA(DisplayName = "Pull")
};
