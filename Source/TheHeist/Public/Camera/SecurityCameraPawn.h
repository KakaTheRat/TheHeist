#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "SecurityCameraPawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDetectedSignature, FVector, DetectionLocation);

UCLASS()
class ASecurityCameraPawn : public APawn
{
    GENERATED_BODY()

public:
    ASecurityCameraPawn();

    UPROPERTY(BlueprintAssignable, Category = "Security Camera")
    FOnPlayerDetectedSignature OnPlayerDetected;

protected:
    virtual void BeginPlay() override;
};