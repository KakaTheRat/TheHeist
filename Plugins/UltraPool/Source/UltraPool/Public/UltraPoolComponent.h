// ============================================================================
// UltraPool - High-Performance Actor Pooling System for Unreal Engine 5
// Author  : Théo de Nanassy
// YouTube : https://www.youtube.com/@UnrealExplorerFR
//
// Copyright (c) 2026 Théo de Nanassy - All rights reserved.
// Distributed via Fab. Unauthorized redistribution is prohibited.
// ============================================================================

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UltraPoolTypes.h"
#include "UltraPoolComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPoolActorAcquired, FPooledActorHandle, Handle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPoolActorReturned, EPoolReturnReason, Reason);

/**
 * Optional component to attach to pooled Actors.
 *
 * Attach this component to any Actor that will be managed by UltraPool to get:
 *   - Automatic return to pool when the owning Actor is destroyed
 *   - Blueprint-accessible delegates (OnAcquired / OnReturned)
 *   - Easy lease queries (GetTimeRemaining, ExtendLease, ForceReturn)
 *
 * This component is NOT required for pooling to work. Any AActor can be pooled.
 */
UCLASS(ClassGroup = (UltraPool), meta = (BlueprintSpawnableComponent), DisplayName = "UltraPool Component")
class ULTRAPOOL_API UUltraPoolComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUltraPoolComponent();

	// ── Configuration ─────────────────────────────────────────────────────────

	/**
	 * Pool tag this actor belongs to.
	 * Must match the PoolTag in the FPoolConfig used to register the pool.
	 * If left None, the pool is looked up by ActorClass.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UltraPool")
	FName AssignedPoolTag;

	// ── Runtime State ─────────────────────────────────────────────────────────

	/** Current lease handle. Invalid when the actor is dormant in the pool. */
	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	FPooledActorHandle CurrentHandle;

	// ── Delegates ─────────────────────────────────────────────────────────────

	UPROPERTY(BlueprintAssignable, Category = "UltraPool")
	FOnPoolActorAcquired OnAcquired;

	UPROPERTY(BlueprintAssignable, Category = "UltraPool")
	FOnPoolActorReturned OnReturned;

	// ── Blueprint API ─────────────────────────────────────────────────────────

	/** Immediately return this actor to its pool. Safe to call from any Blueprint. */
	UFUNCTION(BlueprintCallable, Category = "UltraPool")
	void ForceReturn();

	/** True when this actor currently has an active lease. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UltraPool")
	bool IsPooled() const { return CurrentHandle.IsValidHandle(); }

	/**
	 * Seconds remaining before this lease expires.
	 * Returns 0 if the lease has no expiry (DefaultLeaseDuration == 0).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UltraPool")
	float GetTimeRemaining() const;

	/** Push back the expiry time by ExtraSeconds. */
	UFUNCTION(BlueprintCallable, Category = "UltraPool")
	void ExtendLease(float ExtraSeconds);

	// ── Called by UltraPoolSubsystem — do not call manually ──────────────────

	void NotifyAcquired(const FPooledActorHandle& Handle);
	void NotifyReturned(EPoolReturnReason Reason);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** Guard against double-return in EndPlay vs explicit ForceReturn. */
	bool bReturnGuard = false;
};
