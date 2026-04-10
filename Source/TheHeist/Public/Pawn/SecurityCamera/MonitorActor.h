#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactions/InteractableComponent.h"
#include "Pawn/SecurityCamera/SecurityCamera.h"
#include "MonitorActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMonitorFocused, AMonitorActor*, Monitor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMonitorUnfocused, AMonitorActor*, Monitor);

UCLASS(Blueprintable)
class THEHEIST_API AMonitorActor : public AActor
{
    GENERATED_BODY()

public:
    AMonitorActor();

protected:
    
    virtual void BeginPlay() override;

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monitor|Cameras")
    TArray<ASecurityCamera*> LinkedCameras;

    UFUNCTION(BlueprintCallable, Category = "Monitor")
    void SwipeNext();

    UFUNCTION(BlueprintCallable, Category = "Monitor")
    void SwipePrevious();

    UFUNCTION(BlueprintCallable, Category = "Monitor")
    void SwitchToCamera(int32 Index);

    UFUNCTION(BlueprintPure, Category = "Monitor")
    int32 GetCurrentCameraIndex() const { return CurrentCameraIndex; }

    UFUNCTION(BlueprintPure, Category = "Monitor")
    ASecurityCamera* GetCurrentCamera() const;

    UFUNCTION(BlueprintCallable, Category = "Monitor|Focus")
    void SetFocused(bool bFocused);

    UFUNCTION(BlueprintPure, Category = "Monitor|Focus")
    bool IsFocused() const { return bIsFocused; }

    UFUNCTION(BlueprintCallable, Category = "Monitor|Capture")
    void ActivateCurrentCamera();

    UFUNCTION(BlueprintCallable, Category = "Monitor|Capture")
    void DeactivateCurrentCamera();

    UPROPERTY(BlueprintAssignable, Category = "Monitor|Events")
    FOnMonitorFocused OnMonitorFocused;

    UPROPERTY(BlueprintAssignable, Category = "Monitor|Events")
    FOnMonitorUnfocused OnMonitorUnfocused;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monitor|Components")
    UStaticMeshComponent* ScreenMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monitor|Components")
    UInteractableComponent* InteractableComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monitor|Material")
    UMaterialInterface* MonitorMaterial = nullptr;
    
private:

    UFUNCTION()
    void InitScreenTexture();
    
    UPROPERTY()
    UMaterialInstanceDynamic* DynamicMaterial;

    int32 CurrentCameraIndex = 0;
    bool bIsFocused = false;
    bool bRoomActive = false;

    void UpdateScreenTexture();
};