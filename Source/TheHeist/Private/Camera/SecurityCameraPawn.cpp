#include "Camera/SecurityCameraPawn.h"

#include "EngineUtils.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Guard/GuardAIC.h"

ASecurityCameraPawn::ASecurityCameraPawn()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ASecurityCameraPawn::BeginPlay()
{
    Super::BeginPlay();

}
