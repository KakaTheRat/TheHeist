#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Curves/CurveFloat.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"

#include "SecurityCamera.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHackCamera);

UCLASS(Blueprintable)
class THEHEIST_API ASecurityCamera : public APawn
{
    GENERATED_BODY()

public:
    ASecurityCamera();

protected:
    virtual void BeginPlay() override;
    
    virtual void Tick(float DeltaTime) override;

public:
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    bool bIsMoving = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (EditCondition = "bIsMoving"))
    float RotationSpeed = 30.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (EditCondition = "bIsMoving"))
    float MaxRotationAngle = 60.0f;

    UFUNCTION()
    void HackCamera();

    UPROPERTY(BlueprintAssignable, Category = "Camera")
    FOnHackCamera OnHackCamera;
    
private:
    float CurrentRotation = 0.0f;
    
    bool bRotatingRight = true;
    
    bool bCameraHacked = false;

    FRotator InitialRotation;
};