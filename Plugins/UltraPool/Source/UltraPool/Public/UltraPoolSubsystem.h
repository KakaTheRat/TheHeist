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
#include "Subsystems/WorldSubsystem.h"
#include "UObject/ObjectKey.h"
#include "UltraPoolTypes.h"
#include "UltraPoolSubsystem.generated.h"

struct FUltraPoolData;


/**
 * UltraPool — Core Subsystem
 *
 * One instance per UWorld (automatically created, no manual setup needed).
 * Manages all Actor pools: acquire, return, lease expiry, memory pressure.
 *
 * Access via:
 *   C++:       UUltraPoolSubsystem::Get(this)
 *   Blueprint: UltraPoolBlueprintLibrary functions (recommended)
 */
UCLASS()
class ULTRAPOOL_API UUltraPoolSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	// ── USubsystem ────────────────────────────────────────────────────────────
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// ── FTickableGameObject ───────────────────────────────────────────────────
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;

	// ── Static Helper ─────────────────────────────────────────────────────────
	/** Get the subsystem from any UObject with a valid world. Returns null on dedicated server in v1.0. */
	static UUltraPoolSubsystem* Get(const UObject* WorldContextObject);

	// ── Pool Registration ─────────────────────────────────────────────────────

	/**
	 * Register a pool and optionally warm it up.
	 * Calling this twice with the same Config.ActorClass + Config.PoolTag is safe (idempotent).
	 * @return PoolID to use for Acquire/Return calls. -1 on failure.
	 */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Registration")
	int32 RegisterPool(FPoolConfig Config);

	/** Unregister a pool. If bDestroyActors, all managed actors are destroyed immediately. */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Registration")
	void DeregisterPool(int32 PoolID, bool bDestroyActors = true);

	/** Unregister all pools. */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Registration")
	void DeregisterAllPools(bool bDestroyActors = true);

	/** Pre-spawn all InitialSize actors for the given pool. Call before gameplay if you haven't already. */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Registration")
	void WarmUpPool(int32 PoolID);

	/** Pre-spawn all pools. Useful to call once at level start. */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Registration")
	void WarmUpAllPools();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UltraPool|Registration")
	bool IsPoolRegistered(int32 PoolID) const;

	// ── Acquire ───────────────────────────────────────────────────────────────

	/**
	 * Acquire an actor from the pool.
	 * The actor will be moved to SpawnTransform and returned ready-to-use.
	 * Check Handle.IsValidHandle() before using — a miss returns an invalid handle.
	 */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Acquire",
		meta = (AutoCreateRefTerm = "SpawnTransform"))
	FPooledActorHandle AcquireActor(int32 PoolID, const FTransform& SpawnTransform);

	/** Acquire by actor class. Finds the pool registered for that class (untagged). */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Acquire",
		meta = (AutoCreateRefTerm = "SpawnTransform"))
	FPooledActorHandle AcquireActorByClass(TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform);

	/** Acquire by pool tag (FPoolConfig.PoolTag). */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Acquire",
		meta = (AutoCreateRefTerm = "SpawnTransform"))
	FPooledActorHandle AcquireActorByTag(FName PoolTag, const FTransform& SpawnTransform);

	// ── Return ────────────────────────────────────────────────────────────────

	/** Return an actor to its pool using a handle. Safe to call with an invalid handle (no-op). */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Return")
	void ReturnActor(FPooledActorHandle Handle);

	/**
	 * Return an actor using a direct pointer (slower — requires reverse lookup).
	 * Prefer ReturnActor(Handle) when possible.
	 */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Return")
	void ReturnActorByPointer(AActor* Actor);

	/** Return all active actors for a specific pool. */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Return")
	void ReturnAllActors(int32 PoolID);

	/** Return every active actor across all pools. */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Return")
	void ReturnAllActorsGlobal();

	/** Push back the expiry time of an active lease by ExtraSeconds. */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Return")
	void ExtendLease(FPooledActorHandle Handle, float ExtraSeconds);

	/** Get the remaining lease time in seconds. Returns 0 if no expiry or invalid handle. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UltraPool|Return")
	float GetLeaseTimeRemaining(FPooledActorHandle Handle) const;

	// ── Query ─────────────────────────────────────────────────────────────────

	/** Get stats snapshot for one pool. Returns empty stats if PoolID is not registered. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UltraPool|Stats")
	FPoolRuntimeStats GetPoolStats(int32 PoolID) const;

	/** Get aggregate stats across all pools. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UltraPool|Stats")
	FGlobalPoolStats GetGlobalStats() const;

	/** Get the actor referenced by a handle. Returns null if the handle is invalid or stale. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UltraPool|Query")
	AActor* GetActorFromHandle(FPooledActorHandle Handle) const;

	/** Returns true if the handle is still valid and the lease is active. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UltraPool|Query")
	bool IsHandleValid(FPooledActorHandle Handle) const;

	// ── Memory ────────────────────────────────────────────────────────────────

	/** Shrink a pool to TargetSize by destroying dormant actors. Active actors are never affected. */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Memory")
	void TrimPool(int32 PoolID, int32 TargetSize);

	/** Trim all pools with TrimIdleAfterSeconds configured, if their idle time has elapsed. */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Memory")
	void TrimAllIdlePools();

	/** Grow a pool by Count, pre-spawning actors immediately. */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Memory")
	void GrowPool(int32 PoolID, int32 Count);

	// ── Pool ID Utility ───────────────────────────────────────────────────────

	/** Deterministic pool ID from class + optional tag. Use to cache PoolIDs at startup. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UltraPool|Utility",
		meta = (AutoCreateRefTerm = "Tag"))
	static int32 ComputePoolID(TSubclassOf<AActor> ActorClass, FName Tag);

private:
	// Key: PoolID (computed via ComputePoolID)
	TMap<int32, TSharedPtr<FUltraPoolData>> PoolMap;

	// Secondary indices for lookup-by-class and lookup-by-tag
	TMap<TSubclassOf<AActor>, int32> ClassToPoolID;
	TMap<FName, int32> TagToPoolID;

	// Reverse lookup for ReturnActorByPointer
	// Using FObjectKey for safe actor pointer tracking
	TMap<FObjectKey, int32> ActorToPoolID;

	float SystemUptime = 0.f;

	// ── Internal helpers ──────────────────────────────────────────────────────

	FPooledActorHandle AcquireFromPoolData(FUltraPoolData& PoolData, const FTransform& SpawnTransform);
	void ReturnToPoolData(FUltraPoolData& PoolData, int32 SlotIndex, EPoolReturnReason Reason);
	AActor* SpawnPooledActor(const FPoolConfig& Config);
	void PrepareActorForLease(AActor* Actor, const FTransform& SpawnTransform);
	void PrepareActorForReturn(AActor* Actor, const FPoolConfig& Config, EPoolReturnReason Reason);
	void GrowPoolData(FUltraPoolData& PoolData, int32 Count);
	void ShrinkPoolData(FUltraPoolData& PoolData, int32 TargetSize);
	void ScanLeaseExpiry(float CurrentTime);
	bool ValidateHandleInternal(const FPooledActorHandle& Handle, FUltraPoolData** OutPool) const;
	void RegisterPoolIntoIndex(int32 PoolID, const FPoolConfig& Config);
	void UnregisterPoolFromIndex(int32 PoolID, const FPoolConfig& Config);
};
