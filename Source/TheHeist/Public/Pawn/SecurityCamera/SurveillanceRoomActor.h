#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Pawn/SecurityCamera/MonitorActor.h"
#include "SurveillanceRoomActor.generated.h"

UCLASS(Blueprintable)
class THEHEIST_API ASurveillanceRoomActor : public AActor
{
	GENERATED_BODY()

public:

	ASurveillanceRoomActor();

protected:

	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	TArray<AMonitorActor*> Monitors;

	UPROPERTY(BlueprintReadOnly, Category = "Room")
	AMonitorActor* FocusedMonitor = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Room")
	void FocusMonitor(AMonitorActor* Monitor);

	UFUNCTION(BlueprintCallable, Category = "Room")
	void UnfocusCurrentMonitor();

	UFUNCTION(BlueprintPure, Category = "Room")
	bool HasFocusedMonitor() const { return FocusedMonitor != nullptr; }

private:

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* RoomTrigger;

	bool bPlayerInside = false;

	UFUNCTION()
	void OnPlayerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPlayerExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnMonitorFocused(AMonitorActor* Monitor);

	UFUNCTION()
	void OnMonitorUnfocused(AMonitorActor* Monitor);
};