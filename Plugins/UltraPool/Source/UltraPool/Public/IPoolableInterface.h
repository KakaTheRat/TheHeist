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
#include "UObject/Interface.h"
#include "UltraPoolTypes.h"
#include "IPoolableInterface.generated.h"

/**
 * Optional interface for pooled Actors.
 * Implement on your BP or C++ Actor class to hook into the pool lifecycle.
 *
 * - OnAcquiredFromPool : replaces BeginPlay for state reset. Called each time the actor leaves the pool.
 * - OnReturnedToPool   : called before the actor is hidden. Clean up particles, sounds, timers here.
 * - OnPoolWarmup       : called once at initial spawn. Pre-load heavy assets here.
 * - CanBeReclaimed     : return false to protect this actor from forced reclaim (e.g. during cutscene).
 */

UINTERFACE(MinimalAPI, Blueprintable, Category = "UltraPool")
class UPoolableInterface : public UInterface
{
	GENERATED_BODY()
};

class ULTRAPOOL_API IPoolableInterface
{
	GENERATED_BODY()

public:
	/** Called every time this actor is leased from the pool. Reset your state here. */
	UFUNCTION(BlueprintNativeEvent, Category = "UltraPool")
	void OnAcquiredFromPool(FPooledActorHandle Handle);

	/** Called every time this actor is returned to the pool. */
	UFUNCTION(BlueprintNativeEvent, Category = "UltraPool")
	void OnReturnedToPool(EPoolReturnReason Reason);

	/** Called once when the actor is first spawned for the pool (before any lease). Pre-load assets here. */
	UFUNCTION(BlueprintNativeEvent, Category = "UltraPool")
	void OnPoolWarmup();

	/**
	 * Called when the subsystem wants to forcibly reclaim this actor.
	 * Return false to prevent reclaim (e.g. actor is in a cinematic).
	 * Default implementation returns true.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "UltraPool")
	bool CanBeReclaimed() const;
	virtual bool CanBeReclaimed_Implementation() const { return true; }
};
