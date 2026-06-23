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
#include "Engine/DeveloperSettings.h"
#include "UltraPoolTypes.h"
#include "UltraPoolSettings.generated.h"

/**
 * Project-wide UltraPool settings.
 * Accessible via: Project Settings > Plugins > UltraPool
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "UltraPool"))
class ULTRAPOOL_API UUltraPoolSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UUltraPoolSettings();

	virtual FName GetCategoryName() const override { return FName("Plugins"); }

	// ── General ──────────────────────────────────────────────────────────────

	/** Master switch. When false, the subsystem skips all ticking and logging. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	bool bEnablePooling = true;

	/** Log a warning every time a pool returns a miss (no available actor). */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	bool bLogPoolMisses = false;

	/** Log when a lease expires and the actor is auto-returned. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	bool bLogLeaseExpiry = false;

	// ── Warmup ───────────────────────────────────────────────────────────────

	/**
	 * Maximum actors spawned per frame during WarmUp.
	 * Spread the cost across frames to avoid hitches at level load.
	 * 0 = spawn all synchronously in one frame.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Warmup", meta = (ClampMin = "0"))
	int32 MaxActorsSpawnedPerFrame = 0;

	// ── Memory ───────────────────────────────────────────────────────────────

	/**
	 * Global memory budget in KB. When exceeded, Low and Background priority pools
	 * are automatically trimmed. 0 = no budget limit.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Memory", meta = (ClampMin = "0.0"))
	float GlobalMemoryBudgetKB = 0.f;

	// ── Selection ────────────────────────────────────────────────────────────

	/** Default slot selection policy for all pools. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Selection")
	ESlotSelectionPolicy DefaultSlotPolicy = ESlotSelectionPolicy::LIFO;

	// ── Pre-registered Pools ─────────────────────────────────────────────────

	/**
	 * Pools defined at project level. Automatically registered when the subsystem initializes.
	 * Useful to warm up persistent pools (bullets, VFX) before gameplay starts.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Pools")
	TArray<FPoolConfig> PreregisteredPools;
};
