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
#include "UltraPoolTypes.h"

// ─────────────────────────────────────────────────────────────────────────────
// Internal types — NOT exposed to Blueprint or Fab users
// ─────────────────────────────────────────────────────────────────────────────

/** One slot in a pool. Tracks a single pooled actor and its lease state. */
struct FUltraPoolSlot
{
	TWeakObjectPtr<AActor> Actor;

	/** Incremented every time this slot is returned. Stale handles have a mismatching generation. */
	int32 Generation = 0;

	bool bIsActive = false;

	float LeaseIssuedTime = 0.f;

	/** World time at which the lease expires. 0 = no expiry. */
	float LeaseExpireTime = 0.f;

	/** How many times this specific slot has been leased. */
	int32 AcquireCount = 0;
};

// ─────────────────────────────────────────────────────────────────────────────
// FUltraPoolFreeList
// Manages the indices of available slots with configurable selection policy.
// ─────────────────────────────────────────────────────────────────────────────

class FUltraPoolFreeList
{
public:
	explicit FUltraPoolFreeList(ESlotSelectionPolicy InPolicy = ESlotSelectionPolicy::LIFO)
		: Policy(InPolicy)
	{}

	void SetPolicy(ESlotSelectionPolicy InPolicy) { Policy = InPolicy; }
	ESlotSelectionPolicy GetPolicy() const { return Policy; }

	void Push(int32 SlotIndex, int32 AcquireCount = 0)
	{
		Entries.Add({SlotIndex, AcquireCount});
	}

	/** Returns -1 if empty. */
	int32 Pop()
	{
		if (Entries.IsEmpty()) return -1;

		int32 PickedIndex = 0;

		switch (Policy)
		{
		case ESlotSelectionPolicy::LIFO:
			PickedIndex = Entries.Num() - 1;
			break;

		case ESlotSelectionPolicy::FIFO:
			PickedIndex = 0;
			break;

		case ESlotSelectionPolicy::LeastUsed:
		{
			int32 MinUse = INT_MAX;
			for (int32 i = 0; i < Entries.Num(); i++)
			{
				if (Entries[i].AcquireCount < MinUse)
				{
					MinUse = Entries[i].AcquireCount;
					PickedIndex = i;
				}
			}
			break;
		}

		case ESlotSelectionPolicy::MostUsed:
		{
			int32 MaxUse = -1;
			for (int32 i = 0; i < Entries.Num(); i++)
			{
				if (Entries[i].AcquireCount > MaxUse)
				{
					MaxUse = Entries[i].AcquireCount;
					PickedIndex = i;
				}
			}
			break;
		}
		}

		int32 Result = Entries[PickedIndex].SlotIndex;
		// RemoveAtSwap is O(1): it fills the removed slot with the last element instead of
		// shifting the array. Order is intentionally not preserved here — the selection
		// policy (LIFO/FIFO/LeastUsed/MostUsed) already determined which entry to use, so
		// the remaining order in the backing array is irrelevant until the next Pop.
		// EAllowShrinking was introduced in UE 5.5; use bool (false) on 5.4 and earlier.
		// ULTRAPOOL_ENGINE_MINOR is set by UltraPool.Build.cs from Target.Version.MinorVersion
		// so it is always defined and safe to use in #if across all targets (Editor, Game, etc.).
#if ULTRAPOOL_ENGINE_MINOR >= 5
		Entries.RemoveAtSwap(PickedIndex, 1, EAllowShrinking::No);
#else
		Entries.RemoveAtSwap(PickedIndex, 1, false);
#endif
		return Result;
	}

	void Remove(int32 SlotIndex)
	{
		Entries.RemoveAll([SlotIndex](const FEntry& E){ return E.SlotIndex == SlotIndex; });
	}

	void Reset() { Entries.Reset(); }

	bool IsEmpty() const { return Entries.IsEmpty(); }
	int32 Count() const { return Entries.Num(); }

private:
	struct FEntry
	{
		int32 SlotIndex;
		int32 AcquireCount;
	};

	TArray<FEntry> Entries;
	ESlotSelectionPolicy Policy;
};

// ─────────────────────────────────────────────────────────────────────────────
// FUltraPoolData — full runtime state of one pool
// ─────────────────────────────────────────────────────────────────────────────

struct FUltraPoolData
{
	FPoolConfig Config;
	int32 PoolID = 0;

	TArray<FUltraPoolSlot> Slots;
	FUltraPoolFreeList FreeList;

	/** Reverse lookup: raw actor pointer -> slot index (validated with IsValid before use). */
	TMap<AActor*, int32> ActorToSlotIndex;

	FPoolRuntimeStats Stats;

	float LastTrimCheckTime = 0.f;
	float TotalLeaseTime = 0.f;
	int32 LeaseSampleCount = 0;

	bool bIsWarmedUp = false;
	bool bGrowthInProgress = false; // prevent re-entrant growth

	explicit FUltraPoolData(ESlotSelectionPolicy SlotPolicy = ESlotSelectionPolicy::LIFO)
		: FreeList(SlotPolicy)
	{}
};
