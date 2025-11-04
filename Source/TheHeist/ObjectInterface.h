#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ObjectInterface.generated.h"

/**
 * Interface for usable objects
 */
UINTERFACE(Blueprintable)
class THEHEIST_API UObjectInterface : public UInterface
{
	GENERATED_BODY()
};

class THEHEIST_API IObjectInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon")
	void UseObject();
};
