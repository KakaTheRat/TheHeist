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
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UltraPoolTypes.h"
#include "UltraPoolBlueprintLibrary.generated.h"

/**
 * UltraPool Blueprint Function Library
 *
 * This is the recommended API for Blueprint users.
 * All functions resolve the pool subsystem from WorldContextObject automatically.
 *
 * Quick Start:
 *   1. Call UltraPool_RegisterPool to register your actor class with an initial size.
 *   2. Call UltraPool_Acquire to get an actor. Check bSuccess before using OutActor.
 *   3. Call UltraPool_Return when done — never call Destroy() on pooled actors!
 */
UCLASS()
class ULTRAPOOL_API UUltraPoolBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// ── Registration ──────────────────────────────────────────────────────────

	/**
	 * Register an actor class for pooling.
	 * Safe to call multiple times — subsequent calls with the same class/tag are no-ops.
	 * @return PoolID. Store it for fast acquire/return lookups.
	 */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Registration",
		meta = (WorldContext = "WorldContextObject", DisplayName = "UltraPool Register Pool"))
	static int32 UltraPool_RegisterPool(const UObject* WorldContextObject, FPoolConfig Config);

	/** Unregister a pool. Optionally destroy all managed actors. */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Registration",
		meta = (WorldContext = "WorldContextObject", DisplayName = "UltraPool Deregister Pool"))
	static void UltraPool_DeregisterPool(const UObject* WorldContextObject, int32 PoolID, bool bDestroyActors = true);

	/** Pre-spawn all actors for the given pool. Call before gameplay to avoid hitches. */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Registration",
		meta = (WorldContext = "WorldContextObject", DisplayName = "UltraPool Warm Up Pool"))
	static void UltraPool_WarmUp(const UObject* WorldContextObject, int32 PoolID);

	/** Pre-spawn all registered pools at once. */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Registration",
		meta = (WorldContext = "WorldContextObject", DisplayName = "UltraPool Warm Up All"))
	static void UltraPool_WarmUpAll(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UltraPool|Registration",
		meta = (WorldContext = "WorldContextObject", DisplayName = "UltraPool Is Registered"))
	static bool UltraPool_IsRegistered(const UObject* WorldContextObject, int32 PoolID);

	// ── Acquire ───────────────────────────────────────────────────────────────

	/**
	 * Acquire an actor from the pool by PoolID.
	 * @param bSuccess - false if the pool is exhausted (Fixed policy) or PoolID is invalid.
	 * @param OutActor - the ready-to-use actor. Null if bSuccess is false.
	 * @return Handle - store this and pass it to UltraPool_Return when done.
	 */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Acquire",
		meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "SpawnTransform",
				DisplayName = "UltraPool Acquire"))
	static FPooledActorHandle UltraPool_Acquire(
		const UObject* WorldContextObject,
		int32 PoolID,
		const FTransform& SpawnTransform,
		bool& bSuccess,
		AActor*& OutActor);

	/**
	 * Acquire by actor class (no PoolID needed — slower lookup, fine outside hot path).
	 */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Acquire",
		meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "SpawnTransform",
				DisplayName = "UltraPool Acquire By Class"))
	static FPooledActorHandle UltraPool_AcquireByClass(
		const UObject* WorldContextObject,
		TSubclassOf<AActor> ActorClass,
		const FTransform& SpawnTransform,
		bool& bSuccess,
		AActor*& OutActor);

	/** Acquire by pool tag (FPoolConfig.PoolTag). */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Acquire",
		meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "SpawnTransform",
				DisplayName = "UltraPool Acquire By Tag"))
	static FPooledActorHandle UltraPool_AcquireByTag(
		const UObject* WorldContextObject,
		FName PoolTag,
		const FTransform& SpawnTransform,
		bool& bSuccess,
		AActor*& OutActor);

	// ── Return ────────────────────────────────────────────────────────────────

	/**
	 * Return an actor to its pool.
	 * NEVER call Destroy() on pooled actors. Always use this function instead.
	 */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Return",
		meta = (WorldContext = "WorldContextObject", DisplayName = "UltraPool Return"))
	static void UltraPool_Return(const UObject* WorldContextObject, FPooledActorHandle Handle);

	/** Return using a direct pointer (slower — prefer the handle variant). */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Return",
		meta = (WorldContext = "WorldContextObject", DisplayName = "UltraPool Return By Actor"))
	static void UltraPool_ReturnByActor(const UObject* WorldContextObject, AActor* Actor);

	/** Return all active actors for a specific pool. */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Return",
		meta = (WorldContext = "WorldContextObject", DisplayName = "UltraPool Return All"))
	static void UltraPool_ReturnAll(const UObject* WorldContextObject, int32 PoolID);

	/** Push back the lease expiry by ExtraSeconds. */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Return",
		meta = (WorldContext = "WorldContextObject", DisplayName = "UltraPool Extend Lease"))
	static void UltraPool_ExtendLease(const UObject* WorldContextObject, FPooledActorHandle Handle, float ExtraSeconds);

	// ── Query ─────────────────────────────────────────────────────────────────

	/** Returns true if the handle is still valid and the lease is active. Pure — no world context needed. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UltraPool|Query",
		meta = (DisplayName = "UltraPool Is Handle Valid"))
	static bool UltraPool_IsHandleValid(FPooledActorHandle Handle);

	/** Get the actor from a handle. Returns null if invalid or stale. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UltraPool|Query",
		meta = (WorldContext = "WorldContextObject", DisplayName = "UltraPool Get Actor From Handle"))
	static AActor* UltraPool_GetActor(const UObject* WorldContextObject, FPooledActorHandle Handle);

	/** Get stats for one pool. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UltraPool|Stats",
		meta = (WorldContext = "WorldContextObject", DisplayName = "UltraPool Get Pool Stats"))
	static FPoolRuntimeStats UltraPool_GetPoolStats(const UObject* WorldContextObject, int32 PoolID);

	/** Get aggregate stats across all pools. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UltraPool|Stats",
		meta = (WorldContext = "WorldContextObject", DisplayName = "UltraPool Get Global Stats"))
	static FGlobalPoolStats UltraPool_GetGlobalStats(const UObject* WorldContextObject);

	/** Seconds remaining before this lease expires. 0 = no expiry or invalid handle. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UltraPool|Query",
		meta = (WorldContext = "WorldContextObject", DisplayName = "UltraPool Get Time Remaining"))
	static float UltraPool_GetTimeRemaining(const UObject* WorldContextObject, FPooledActorHandle Handle);

	// ── Utility ───────────────────────────────────────────────────────────────

	/**
	 * Convenience constructor for common pool configs.
	 * Pure function — usable anywhere without a world context.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UltraPool|Utility",
		meta = (DisplayName = "UltraPool Make Config",
				AutoCreateRefTerm = "Tag"))
	static FPoolConfig UltraPool_MakeConfig(
		TSubclassOf<AActor> ActorClass,
		int32 InitialSize = 10,
		int32 MaxSize = 0,
		float LeaseDuration = 0.f,
		EPoolGrowthPolicy GrowthPolicy = EPoolGrowthPolicy::OnDemand,
		FName Tag = NAME_None);

	/** Get the deterministic PoolID for a class/tag pair. Use to cache IDs at startup. Pure. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UltraPool|Utility",
		meta = (DisplayName = "UltraPool Compute Pool ID",
				AutoCreateRefTerm = "Tag"))
	static int32 UltraPool_ComputePoolID(TSubclassOf<AActor> ActorClass, FName Tag);

	/** Human-readable handle string for debugging. Pure. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UltraPool|Utility",
		meta = (DisplayName = "UltraPool Handle To String"))
	static FString UltraPool_HandleToString(FPooledActorHandle Handle);

	/** Trim all idle pools (respects TrimIdleAfterSeconds config per pool). */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Memory",
		meta = (WorldContext = "WorldContextObject", DisplayName = "UltraPool Trim All"))
	static void UltraPool_TrimAll(const UObject* WorldContextObject);

	/** Grow a pool by Count, spawning actors immediately. */
	UFUNCTION(BlueprintCallable, Category = "UltraPool|Memory",
		meta = (WorldContext = "WorldContextObject", DisplayName = "UltraPool Force Grow"))
	static void UltraPool_ForceGrow(const UObject* WorldContextObject, int32 PoolID, int32 Count);

private:
	static class UUltraPoolSubsystem* GetSubsystem(const UObject* WorldContextObject);
};
