// AlertTypes.h
#pragma once

#include "CoreMinimal.h"
#include "AlertTypes.generated.h"

UENUM(BlueprintType)
enum class EAlertTypes : uint8
{
	Visual     UMETA(DisplayName = "Visual"),      
	Auditive   UMETA(DisplayName = "Auditive"),    
	Suspicious UMETA(DisplayName = "Suspicious")   
};