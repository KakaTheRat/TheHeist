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
#include "Templates/SubclassOf.h"

class AActor;

#include "UltraPoolTypes.generated.h"

// ─────────────────────────────────────────────────────────────────────────────
// Enumerations
// ─────────────────────────────────────────────────────────────────────────────

UENUM(BlueprintType)
enum class EPoolGrowthPolicy : uint8
{
	Fixed      UMETA(DisplayName = "Fixed (Never Grow)"),
	OnDemand   UMETA(DisplayName = "On Demand (+1 per miss)"),
	Elastic    UMETA(DisplayName = "Elastic (+N per miss)"),
	Doubling   UMETA(DisplayName = "Doubling (x2 per miss)")
};

UENUM(BlueprintType)
enum class EPoolPriority : uint8
{
	Critical   UMETA(DisplayName = "Critical (never trimmed)"),
	High       UMETA(DisplayName = "High"),
	Normal     UMETA(DisplayName = "Normal"),
	Low        UMETA(DisplayName = "Low (trimmed first)"),
	Background UMETA(DisplayName = "Background (trimmed aggressively)")
};

UENUM(BlueprintType)
enum class EPoolReturnReason : uint8
{
	Explicit        UMETA(DisplayName = "Explicit"),
	LeaseExpired    UMETA(DisplayName = "Lease Expired"),
	PoolOverflow    UMETA(DisplayName = "Pool Overflow"),
	LevelTransition UMETA(DisplayName = "Level Transition"),
	OwnerDestroyed  UMETA(DisplayName = "Owner Destroyed"),
	ForceReclaim    UMETA(DisplayName = "Force Reclaim")
};

UENUM(BlueprintType)
enum class ESlotSelectionPolicy : uint8
{
	LIFO      UMETA(DisplayName = "LIFO (Best Cache Locality)"),
	FIFO      UMETA(DisplayName = "FIFO (Even Distribution)"),
	LeastUsed UMETA(DisplayName = "Least Used (Wear Leveling)"),
	MostUsed  UMETA(DisplayName = "Most Used (Hot Cache)")
};

// ─────────────────────────────────────────────────────────────────────────────
// FPooledActorHandle
// Lightweight value type identifying one lease. Safe to store in Blueprint.
// The Generation field makes stale handles detectable after the slot is reused.
// ─────────────────────────────────────────────────────────────────────────────

USTRUCT(BlueprintType)
struct ULTRAPOOL_API FPooledActorHandle
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	int32 PoolID = 0;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	int32 ActorIndex = -1;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	int32 Generation = 0;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	bool bIsValid = false;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	float LeaseTime = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	FName PoolTag;

	bool IsValidHandle() const { return bIsValid && ActorIndex >= 0; }

	void Invalidate()
	{
		bIsValid = false;
		ActorIndex = -1;
		PoolID = 0;
		Generation = 0;
	}

	bool operator==(const FPooledActorHandle& Other) const
	{
		return PoolID == Other.PoolID
			&& ActorIndex == Other.ActorIndex
			&& Generation == Other.Generation;
	}

	bool operator!=(const FPooledActorHandle& Other) const { return !(*this == Other); }

	FString ToString() const
	{
		return FString::Printf(TEXT("Handle[Pool:%d Idx:%d Gen:%d Valid:%s Tag:%s]"),
			PoolID, ActorIndex, Generation,
			bIsValid ? TEXT("Y") : TEXT("N"),
			*PoolTag.ToString());
	}
};

// ─────────────────────────────────────────────────────────────────────────────
// FPoolConfig
// Full configuration for one pool. Pass to RegisterPool / RegisterPoolByTag.
// ─────────────────────────────────────────────────────────────────────────────

USTRUCT(BlueprintType)
struct ULTRAPOOL_API FPoolConfig
{
	GENERATED_BODY()

	/** Actor class this pool manages. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UltraPool")
	TSubclassOf<AActor> ActorClass;

	/** Actors pre-spawned during WarmUp. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UltraPool", meta = (ClampMin = "0"))
	int32 InitialSize = 10;

	/** Hard ceiling. 0 = unlimited (use with caution on Doubling policy). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UltraPool", meta = (ClampMin = "0"))
	int32 MaxSize = 0;

	/** Default lease duration in seconds. 0 = no expiry. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UltraPool", meta = (ClampMin = "0.0"))
	float DefaultLeaseDuration = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UltraPool")
	EPoolGrowthPolicy GrowthPolicy = EPoolGrowthPolicy::OnDemand;

	/** How many actors to add when Elastic policy triggers. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UltraPool", meta = (ClampMin = "1", EditCondition = "GrowthPolicy == EPoolGrowthPolicy::Elastic"))
	int32 GrowthIncrement = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UltraPool")
	EPoolPriority Priority = EPoolPriority::Normal;

	/** Logical name for Blueprint lookup via AcquireByTag. Leave None for class-only lookup. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UltraPool")
	FName PoolTag;

	/** If true, the subsystem calls ReturnActor automatically when the lease expires. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UltraPool")
	bool bAutoReturnOnLeaseExpiry = true;

	/** Teleport actor to world origin area on return. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UltraPool")
	bool bResetTransformOnReturn = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UltraPool")
	bool bDisableCollisionOnReturn = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UltraPool")
	bool bDisableTickOnReturn = true;

	/** Shrink pool if actors have been idle longer than this. 0 = no auto-trim. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UltraPool", meta = (ClampMin = "0.0"))
	float TrimIdleAfterSeconds = 0.f;

	/** Target size after trim. 0 = trim to current active count. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UltraPool", meta = (ClampMin = "0"))
	int32 TrimTargetCount = 0;
};

// ─────────────────────────────────────────────────────────────────────────────
// FPoolRuntimeStats — snapshot of one pool, safe to read in Blueprint
// ─────────────────────────────────────────────────────────────────────────────

USTRUCT(BlueprintType)
struct ULTRAPOOL_API FPoolRuntimeStats
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	FName PoolTag;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	TSubclassOf<AActor> ActorClass;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	int32 TotalSize = 0;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	int32 ActiveCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	int32 AvailableCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	int32 PeakActiveCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	int32 TotalAcquireCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	int32 TotalReturnCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	int32 TotalSpawnCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	int32 MissCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	float AverageLeaseTime = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	EPoolPriority Priority = EPoolPriority::Normal;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	EPoolGrowthPolicy GrowthPolicy = EPoolGrowthPolicy::OnDemand;
};

// ─────────────────────────────────────────────────────────────────────────────
// FGlobalPoolStats — aggregate across all pools
// ─────────────────────────────────────────────────────────────────────────────

USTRUCT(BlueprintType)
struct ULTRAPOOL_API FGlobalPoolStats
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	int32 RegisteredPoolCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	int32 TotalActorsManaged = 0;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	int32 TotalActiveLeases = 0;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	int32 TotalAvailableActors = 0;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	int32 TotalMisses = 0;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	float SystemUptimeSeconds = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "UltraPool")
	TArray<FPoolRuntimeStats> PerPoolStats;
};
