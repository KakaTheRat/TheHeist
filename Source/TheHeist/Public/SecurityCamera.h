// C++
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "SecurityCamera.generated.h"

UENUM(BlueprintType)
enum class ECameraSweepMode : uint8
{
    None     UMETA(DisplayName = "None"),
    PingPong UMETA(DisplayName = "PingPong"),
    Constant UMETA(DisplayName = "Constant")
};

UCLASS(Blueprintable)
class THEHEIST_API ASecurityCamera : public APawn
{
    GENERATED_BODY()

public:
    ASecurityCamera();

protected:
    virtual void BeginPlay() override;

public:
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    ECameraSweepMode SweepMode = ECameraSweepMode::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (EditCondition = "SweepMode != ECameraSweepMode::None"))
    float SweepAngle = 45.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (EditCondition = "SweepMode != ECameraSweepMode::None"))
    float SweepSpeed = 30.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (EditCondition = "SweepMode == ECameraSweepMode::PingPong"))
    UCurveFloat* SweepCurve;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    FRotator InitialRotation = FRotator::ZeroRotator;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    bool bIsActive = true;

    UFUNCTION(BlueprintCallable, Category = "Camera")
    void SetActive(bool bEnable);

private:
    FRotator BaseRotation;
};
