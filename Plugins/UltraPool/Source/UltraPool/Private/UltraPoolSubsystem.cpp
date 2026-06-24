// ============================================================================
// UltraPool - High-Performance Actor Pooling System for Unreal Engine 5
// Author  : Théo de Nanassy
// YouTube : https://www.youtube.com/@UnrealExplorerFR
//
// Copyright (c) 2026 Théo de Nanassy - All rights reserved.
// Distributed via Fab. Unauthorized redistribution is prohibited.
// ============================================================================

#include "UltraPoolSubsystem.h"
#include "UltraPoolInternal.h"
#include "UltraPoolSettings.h"
#include "UltraPoolComponent.h"
#include "IPoolableInterface.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

DEFINE_LOG_CATEGORY_STATIC(LogUltraPool, Log, All);

// ─────────────────────────────────────────────────────────────────────────────
// USubsystem
// ─────────────────────────────────────────────────────────────────────────────

void UUltraPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Register pools defined in Project Settings
	const UUltraPoolSettings* Settings = GetDefault<UUltraPoolSettings>();
	if (Settings && Settings->bEnablePooling)
	{
		for (const FPoolConfig& Config : Settings->PreregisteredPools)
		{
			if (Config.ActorClass)
			{
				RegisterPool(Config);
			}
		}
	}
}

void UUltraPoolSubsystem::Deinitialize()
{
	// Return all active leases then destroy actors
	DeregisterAllPools(true);

	Super::Deinitialize();
}

// ─────────────────────────────────────────────────────────────────────────────
// FTickableGameObject
// ─────────────────────────────────────────────────────────────────────────────

void UUltraPoolSubsystem::Tick(float DeltaTime)
{
	const UUltraPoolSettings* Settings = GetDefault<UUltraPoolSettings>();
	if (!Settings || !Settings->bEnablePooling) return;

	SystemUptime += DeltaTime;

	const float CurrentTime = GetWorld()->GetTimeSeconds();

	// Scan for expired leases
	ScanLeaseExpiry(CurrentTime);

	// Idle trim check
	TrimAllIdlePools();
}

TStatId UUltraPoolSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UUltraPoolSubsystem, STATGROUP_Tickables);
}

bool UUltraPoolSubsystem::IsTickable() const
{
	return !PoolMap.IsEmpty() && !IsTemplate();
}

// ─────────────────────────────────────────────────────────────────────────────
// Static Helper
// ─────────────────────────────────────────────────────────────────────────────

UUltraPoolSubsystem* UUltraPoolSubsystem::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;
	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return nullptr;
	return World->GetSubsystem<UUltraPoolSubsystem>();
}

// ─────────────────────────────────────────────────────────────────────────────
// Pool Registration
// ─────────────────────────────────────────────────────────────────────────────

int32 UUltraPoolSubsystem::RegisterPool(FPoolConfig Config)
{
	if (!Config.ActorClass)
	{
		UE_LOG(LogUltraPool, Warning, TEXT("RegisterPool: ActorClass is null. Skipping."));
		return -1;
	}

	const int32 PoolID = ComputePoolID(Config.ActorClass, Config.PoolTag);

	// Idempotent: if already registered, return existing ID
	if (PoolMap.Contains(PoolID))
	{
		return PoolID;
	}

	const UUltraPoolSettings* Settings = GetDefault<UUltraPoolSettings>();
	const ESlotSelectionPolicy Policy = Settings ? Settings->DefaultSlotPolicy : ESlotSelectionPolicy::LIFO;

	TSharedPtr<FUltraPoolData> PoolData = MakeShared<FUltraPoolData>(Policy);
	PoolData->Config = Config;
	PoolData->PoolID = PoolID;
	PoolData->Stats.PoolTag = Config.PoolTag;
	PoolData->Stats.ActorClass = Config.ActorClass;
	PoolData->Stats.Priority = Config.Priority;
	PoolData->Stats.GrowthPolicy = Config.GrowthPolicy;

	PoolMap.Add(PoolID, PoolData);
	RegisterPoolIntoIndex(PoolID, Config);

	// Warm up immediately
	if (Config.InitialSize > 0)
	{
		WarmUpPool(PoolID);
	}

	return PoolID;
}

void UUltraPoolSubsystem::DeregisterPool(int32 PoolID, bool bDestroyActors)
{
	TSharedPtr<FUltraPoolData>* FoundPool = PoolMap.Find(PoolID);
	if (!FoundPool) return;

	FUltraPoolData& PoolData = **FoundPool;

	// Return all active leases first
	ReturnAllActors(PoolID);

	if (bDestroyActors)
	{
		for (FUltraPoolSlot& Slot : PoolData.Slots)
		{
			if (AActor* Actor = Slot.Actor.Get())
			{
				// Remove from reverse map before destroy
				ActorToPoolID.Remove(FObjectKey(Actor));
				Actor->Destroy();
			}
		}
	}

	UnregisterPoolFromIndex(PoolID, PoolData.Config);
	PoolMap.Remove(PoolID);
}

void UUltraPoolSubsystem::DeregisterAllPools(bool bDestroyActors)
{
	TArray<int32> PoolIDs;
	PoolMap.GetKeys(PoolIDs);

	for (int32 ID : PoolIDs)
	{
		DeregisterPool(ID, bDestroyActors);
	}
}

void UUltraPoolSubsystem::WarmUpPool(int32 PoolID)
{
	TSharedPtr<FUltraPoolData>* FoundPool = PoolMap.Find(PoolID);
	if (!FoundPool) return;

	FUltraPoolData& PoolData = **FoundPool;
	if (PoolData.bIsWarmedUp) return;

	const int32 ToSpawn = PoolData.Config.InitialSize - PoolData.Slots.Num();
	if (ToSpawn > 0)
	{
		GrowPoolData(PoolData, ToSpawn);
	}

	PoolData.bIsWarmedUp = true;
}

void UUltraPoolSubsystem::WarmUpAllPools()
{
	for (auto& Pair : PoolMap)
	{
		WarmUpPool(Pair.Key);
	}
}

bool UUltraPoolSubsystem::IsPoolRegistered(int32 PoolID) const
{
	return PoolMap.Contains(PoolID);
}

// ─────────────────────────────────────────────────────────────────────────────
// Acquire
// ─────────────────────────────────────────────────────────────────────────────

FPooledActorHandle UUltraPoolSubsystem::AcquireActor(int32 PoolID, const FTransform& SpawnTransform)
{
	TSharedPtr<FUltraPoolData>* FoundPool = PoolMap.Find(PoolID);
	if (!FoundPool)
	{
		UE_LOG(LogUltraPool, Warning, TEXT("AcquireActor: PoolID %d not registered."), PoolID);
		return FPooledActorHandle{};
	}

	return AcquireFromPoolData(**FoundPool, SpawnTransform);
}

FPooledActorHandle UUltraPoolSubsystem::AcquireActorByClass(TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform)
{
	if (!ActorClass) return FPooledActorHandle{};

	const int32* FoundID = ClassToPoolID.Find(ActorClass);
	if (!FoundID)
	{
		UE_LOG(LogUltraPool, Warning, TEXT("AcquireActorByClass: No pool registered for class '%s'."), *ActorClass->GetName());
		return FPooledActorHandle{};
	}

	return AcquireActor(*FoundID, SpawnTransform);
}

FPooledActorHandle UUltraPoolSubsystem::AcquireActorByTag(FName PoolTag, const FTransform& SpawnTransform)
{
	if (PoolTag.IsNone()) return FPooledActorHandle{};

	const int32* FoundID = TagToPoolID.Find(PoolTag);
	if (!FoundID)
	{
		UE_LOG(LogUltraPool, Warning, TEXT("AcquireActorByTag: No pool registered with tag '%s'."), *PoolTag.ToString());
		return FPooledActorHandle{};
	}

	return AcquireActor(*FoundID, SpawnTransform);
}

// ─────────────────────────────────────────────────────────────────────────────
// Return
// ─────────────────────────────────────────────────────────────────────────────

void UUltraPoolSubsystem::ReturnActor(FPooledActorHandle Handle)
{
	if (!Handle.IsValidHandle()) return;

	FUltraPoolData* PoolData = nullptr;
	if (!ValidateHandleInternal(Handle, &PoolData) || !PoolData) return;

	ReturnToPoolData(*PoolData, Handle.ActorIndex, EPoolReturnReason::Explicit);
}

void UUltraPoolSubsystem::ReturnActorByPointer(AActor* Actor)
{
	if (!IsValid(Actor)) return;

	const int32* FoundPoolID = ActorToPoolID.Find(FObjectKey(Actor));
	if (!FoundPoolID) return;

	TSharedPtr<FUltraPoolData>* FoundPool = PoolMap.Find(*FoundPoolID);
	if (!FoundPool) return;

	FUltraPoolData& PoolData = **FoundPool;
	const int32* FoundSlot = PoolData.ActorToSlotIndex.Find(Actor);
	if (!FoundSlot) return;

	ReturnToPoolData(PoolData, *FoundSlot, EPoolReturnReason::Explicit);
}

void UUltraPoolSubsystem::ReturnAllActors(int32 PoolID)
{
	TSharedPtr<FUltraPoolData>* FoundPool = PoolMap.Find(PoolID);
	if (!FoundPool) return;

	FUltraPoolData& PoolData = **FoundPool;

	for (int32 i = 0; i < PoolData.Slots.Num(); i++)
	{
		if (PoolData.Slots[i].bIsActive)
		{
			ReturnToPoolData(PoolData, i, EPoolReturnReason::ForceReclaim);
		}
	}
}

void UUltraPoolSubsystem::ReturnAllActorsGlobal()
{
	for (auto& Pair : PoolMap)
	{
		ReturnAllActors(Pair.Key);
	}
}

void UUltraPoolSubsystem::ExtendLease(FPooledActorHandle Handle, float ExtraSeconds)
{
	if (!Handle.IsValidHandle() || ExtraSeconds <= 0.f) return;

	FUltraPoolData* PoolData = nullptr;
	if (!ValidateHandleInternal(Handle, &PoolData) || !PoolData) return;

	FUltraPoolSlot& Slot = PoolData->Slots[Handle.ActorIndex];
	if (Slot.LeaseExpireTime > 0.f)
	{
		Slot.LeaseExpireTime += ExtraSeconds;
	}
}

float UUltraPoolSubsystem::GetLeaseTimeRemaining(FPooledActorHandle Handle) const
{
	if (!Handle.IsValidHandle()) return 0.f;

	FUltraPoolData* PoolData = nullptr;
	if (!ValidateHandleInternal(Handle, &PoolData) || !PoolData) return 0.f;

	const FUltraPoolSlot& Slot = PoolData->Slots[Handle.ActorIndex];
	if (Slot.LeaseExpireTime <= 0.f) return 0.f;

	const float Remaining = Slot.LeaseExpireTime - GetWorld()->GetTimeSeconds();
	return FMath::Max(0.f, Remaining);
}

// ─────────────────────────────────────────────────────────────────────────────
// Query
// ─────────────────────────────────────────────────────────────────────────────

FPoolRuntimeStats UUltraPoolSubsystem::GetPoolStats(int32 PoolID) const
{
	const TSharedPtr<FUltraPoolData>* FoundPool = PoolMap.Find(PoolID);
	if (!FoundPool) return FPoolRuntimeStats{};
	return (*FoundPool)->Stats;
}

FGlobalPoolStats UUltraPoolSubsystem::GetGlobalStats() const
{
	FGlobalPoolStats Global;
	Global.SystemUptimeSeconds = SystemUptime;
	Global.RegisteredPoolCount = PoolMap.Num();

	for (const auto& Pair : PoolMap)
	{
		const FPoolRuntimeStats& S = Pair.Value->Stats;
		Global.TotalActorsManaged    += S.TotalSize;
		Global.TotalActiveLeases     += S.ActiveCount;
		Global.TotalAvailableActors  += S.AvailableCount;
		Global.TotalMisses           += S.MissCount;
		Global.PerPoolStats.Add(S);
	}

	return Global;
}

AActor* UUltraPoolSubsystem::GetActorFromHandle(FPooledActorHandle Handle) const
{
	if (!Handle.IsValidHandle()) return nullptr;

	FUltraPoolData* PoolData = nullptr;
	if (!ValidateHandleInternal(Handle, &PoolData) || !PoolData) return nullptr;

	return PoolData->Slots[Handle.ActorIndex].Actor.Get();
}

bool UUltraPoolSubsystem::IsHandleValid(FPooledActorHandle Handle) const
{
	if (!Handle.IsValidHandle()) return false;
	FUltraPoolData* PoolData = nullptr;
	return ValidateHandleInternal(Handle, &PoolData);
}

// ─────────────────────────────────────────────────────────────────────────────
// Memory
// ─────────────────────────────────────────────────────────────────────────────

void UUltraPoolSubsystem::TrimPool(int32 PoolID, int32 TargetSize)
{
	TSharedPtr<FUltraPoolData>* FoundPool = PoolMap.Find(PoolID);
	if (!FoundPool) return;

	ShrinkPoolData(**FoundPool, TargetSize);
}

void UUltraPoolSubsystem::TrimAllIdlePools()
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();

	for (auto& Pair : PoolMap)
	{
		FUltraPoolData& PoolData = *Pair.Value;
		const float IdleThreshold = PoolData.Config.TrimIdleAfterSeconds;

		if (IdleThreshold <= 0.f) continue;
		if ((CurrentTime - PoolData.LastTrimCheckTime) < IdleThreshold) continue;

		PoolData.LastTrimCheckTime = CurrentTime;

		const int32 Target = (PoolData.Config.TrimTargetCount > 0)
			? PoolData.Config.TrimTargetCount
			: PoolData.Stats.ActiveCount;

		ShrinkPoolData(PoolData, Target);
	}
}

void UUltraPoolSubsystem::GrowPool(int32 PoolID, int32 Count)
{
	TSharedPtr<FUltraPoolData>* FoundPool = PoolMap.Find(PoolID);
	if (!FoundPool || Count <= 0) return;
	GrowPoolData(**FoundPool, Count);
}

// ─────────────────────────────────────────────────────────────────────────────
// Pool ID Utility
// ─────────────────────────────────────────────────────────────────────────────

int32 UUltraPoolSubsystem::ComputePoolID(TSubclassOf<AActor> ActorClass, FName Tag)
{
	if (!ActorClass) return -1;

	// Combine class name hash and tag hash for a stable, deterministic ID
	const uint32 ClassHash = GetTypeHash(ActorClass->GetFName());
	const uint32 TagHash   = GetTypeHash(Tag);
	const uint32 Combined  = HashCombine(ClassHash, TagHash);

	// Cast to int32 safely (avoid negative values from high bit)
	return static_cast<int32>(Combined & 0x7FFFFFFF);
}

// ─────────────────────────────────────────────────────────────────────────────
// Internal — Acquire
// ─────────────────────────────────────────────────────────────────────────────

FPooledActorHandle UUltraPoolSubsystem::AcquireFromPoolData(FUltraPoolData& PoolData, const FTransform& SpawnTransform)
{
	FPooledActorHandle Handle;
	const UUltraPoolSettings* Settings = GetDefault<UUltraPoolSettings>();
	const float CurrentTime = GetWorld()->GetTimeSeconds();

	// Try to get a free slot
	int32 SlotIndex = PoolData.FreeList.Pop();

	if (SlotIndex == -1)
	{
		// Pool exhausted — apply growth policy
		if (PoolData.bGrowthInProgress)
		{
			// Re-entrant growth guard: shouldn't normally happen
			PoolData.Stats.MissCount++;
			if (Settings && Settings->bLogPoolMisses)
			{
				UE_LOG(LogUltraPool, Warning, TEXT("Pool '%s' miss (growth in progress)."),
					*PoolData.Config.PoolTag.ToString());
			}
			return Handle;
		}

		const int32 MaxSize = PoolData.Config.MaxSize;

		switch (PoolData.Config.GrowthPolicy)
		{
		case EPoolGrowthPolicy::Fixed:
			PoolData.Stats.MissCount++;
			if (Settings && Settings->bLogPoolMisses)
			{
				UE_LOG(LogUltraPool, Warning, TEXT("Pool '%s' miss (Fixed policy, size=%d)."),
					*PoolData.Config.PoolTag.ToString(), PoolData.Slots.Num());
			}
			return Handle;

		case EPoolGrowthPolicy::OnDemand:
			GrowPoolData(PoolData, 1);
			break;

		case EPoolGrowthPolicy::Elastic:
			GrowPoolData(PoolData, PoolData.Config.GrowthIncrement);
			break;

		case EPoolGrowthPolicy::Doubling:
		{
			const int32 CurrentTotal = PoolData.Slots.Num();
			const int32 GrowCount = FMath::Max(1, CurrentTotal);

			if (MaxSize > 0 && CurrentTotal + GrowCount > MaxSize)
			{
				PoolData.Stats.MissCount++;
				return Handle;
			}
			if (CurrentTotal + GrowCount > 10000)
			{
				UE_LOG(LogUltraPool, Error, TEXT("Pool '%s' would exceed 10,000 actors cap! Refusing growth."),
					*PoolData.Config.PoolTag.ToString());
				PoolData.Stats.MissCount++;
				return Handle;
			}
			GrowPoolData(PoolData, GrowCount);
			break;
		}
		}

		SlotIndex = PoolData.FreeList.Pop();
	}

	if (SlotIndex < 0 || SlotIndex >= PoolData.Slots.Num())
	{
		PoolData.Stats.MissCount++;
		return Handle;
	}

	// Validate max size constraint
	if (PoolData.Config.MaxSize > 0 && PoolData.Stats.ActiveCount >= PoolData.Config.MaxSize)
	{
		// Return the slot we just took before bailing
		PoolData.FreeList.Push(SlotIndex, PoolData.Slots[SlotIndex].AcquireCount);
		PoolData.Stats.MissCount++;
		return Handle;
	}

	FUltraPoolSlot& Slot = PoolData.Slots[SlotIndex];

	// Actor might have been GC'd externally (e.g. level transition, manual Destroy call)
	AActor* Actor = Slot.Actor.Get();
	if (!IsValid(Actor))
	{
		// The stale ActorToSlotIndex entry for the GC'd pointer is intentionally left in
		// the map. Since the raw pointer is gone, it can never match a future lookup
		// (raw pointer addresses are not reused by UE's GC in the same frame). The
		// replacement actor is registered fresh with its own pointer below.
		Actor = SpawnPooledActor(PoolData.Config);
		if (!IsValid(Actor))
		{
			PoolData.Stats.MissCount++;
			return Handle;
		}
		Slot.Actor = Actor;
		PoolData.Stats.TotalSpawnCount++;
		PoolData.ActorToSlotIndex.Add(Actor, SlotIndex);
		ActorToPoolID.Add(FObjectKey(Actor), PoolData.PoolID);
	}

	// Lease the slot
	Slot.bIsActive = true;
	Slot.LeaseIssuedTime = CurrentTime;
	Slot.LeaseExpireTime = (PoolData.Config.DefaultLeaseDuration > 0.f)
		? (CurrentTime + PoolData.Config.DefaultLeaseDuration)
		: 0.f;
	Slot.AcquireCount++;

	PrepareActorForLease(Actor, SpawnTransform);

	// Build handle
	Handle.PoolID      = PoolData.PoolID;
	Handle.ActorIndex  = SlotIndex;
	Handle.Generation  = Slot.Generation;
	Handle.bIsValid    = true;
	Handle.LeaseTime   = CurrentTime;
	Handle.PoolTag     = PoolData.Config.PoolTag;

	// Update stats
	PoolData.Stats.TotalAcquireCount++;
	PoolData.Stats.ActiveCount++;
	PoolData.Stats.AvailableCount = PoolData.FreeList.Count();
	if (PoolData.Stats.ActiveCount > PoolData.Stats.PeakActiveCount)
	{
		PoolData.Stats.PeakActiveCount = PoolData.Stats.ActiveCount;
	}

	// Notify component if present
	if (UUltraPoolComponent* PoolComp = Actor->FindComponentByClass<UUltraPoolComponent>())
	{
		PoolComp->NotifyAcquired(Handle);
	}

	// Notify interface if implemented
	if (Actor->Implements<UPoolableInterface>())
	{
		IPoolableInterface::Execute_OnAcquiredFromPool(Actor, Handle);
	}

	return Handle;
}

// ─────────────────────────────────────────────────────────────────────────────
// Internal — Return
// ─────────────────────────────────────────────────────────────────────────────

void UUltraPoolSubsystem::ReturnToPoolData(FUltraPoolData& PoolData, int32 SlotIndex, EPoolReturnReason Reason)
{
	if (!PoolData.Slots.IsValidIndex(SlotIndex)) return;

	FUltraPoolSlot& Slot = PoolData.Slots[SlotIndex];
	if (!Slot.bIsActive) return; // Already returned — no-op

	AActor* Actor = Slot.Actor.Get();

	// Notify before hiding
	if (IsValid(Actor))
	{
		if (Actor->Implements<UPoolableInterface>())
		{
			IPoolableInterface::Execute_OnReturnedToPool(Actor, Reason);
		}
		if (UUltraPoolComponent* PoolComp = Actor->FindComponentByClass<UUltraPoolComponent>())
		{
			PoolComp->NotifyReturned(Reason);
		}

		PrepareActorForReturn(Actor, PoolData.Config, Reason);
	}

	// Update lease time average
	if (Slot.LeaseIssuedTime > 0.f)
	{
		const float LeaseDuration = GetWorld()->GetTimeSeconds() - Slot.LeaseIssuedTime;
		PoolData.TotalLeaseTime += LeaseDuration;
		PoolData.LeaseSampleCount++;
		PoolData.Stats.AverageLeaseTime = PoolData.TotalLeaseTime / static_cast<float>(PoolData.LeaseSampleCount);
	}

	// Reset slot
	Slot.bIsActive = false;
	Slot.LeaseIssuedTime = 0.f;
	Slot.LeaseExpireTime = 0.f;
	Slot.Generation++;    // Invalidate any existing handles pointing at this slot

	// Push back to free list
	PoolData.FreeList.Push(SlotIndex, Slot.AcquireCount);

	// Update stats
	PoolData.Stats.TotalReturnCount++;
	PoolData.Stats.ActiveCount  = FMath::Max(0, PoolData.Stats.ActiveCount - 1);
	PoolData.Stats.AvailableCount = PoolData.FreeList.Count();
}

// ─────────────────────────────────────────────────────────────────────────────
// Internal — Spawn & Prepare
// ─────────────────────────────────────────────────────────────────────────────

AActor* UUltraPoolSubsystem::SpawnPooledActor(const FPoolConfig& Config)
{
	UWorld* World = GetWorld();
	if (!World || !Config.ActorClass) return nullptr;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn deferred so we can call OnPoolWarmup before BeginPlay
	AActor* Actor = World->SpawnActorDeferred<AActor>(
		Config.ActorClass,
		FTransform(FVector(0.f, 0.f, -100000.f)), // park far below world
		nullptr, nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!IsValid(Actor)) return nullptr;

	// Notify interface before BeginPlay
	if (Actor->Implements<UPoolableInterface>())
	{
		IPoolableInterface::Execute_OnPoolWarmup(Actor);
	}

	Actor->FinishSpawning(FTransform(FVector(0.f, 0.f, -100000.f)));

	// Bug fix: SetActorHiddenInGame MUST be called after FinishSpawning. If a Blueprint actor
	// has a Construction Script, FinishSpawning will re-run it, which resets bHidden back to
	// false. Calling hide before FinishSpawning would be silently overridden every time.
	Actor->SetActorHiddenInGame(true);
	if (Config.bDisableCollisionOnReturn)
	{
		Actor->SetActorEnableCollision(false);
	}
	if (Config.bDisableTickOnReturn)
	{
		Actor->SetActorTickEnabled(false);
	}

	return Actor;
}

void UUltraPoolSubsystem::PrepareActorForLease(AActor* Actor, const FTransform& SpawnTransform)
{
	if (!IsValid(Actor)) return;

	Actor->SetActorTransform(SpawnTransform, false, nullptr, ETeleportType::TeleportPhysics);
	Actor->SetActorHiddenInGame(false);
	Actor->SetActorEnableCollision(true);
	Actor->SetActorTickEnabled(true);
}

void UUltraPoolSubsystem::PrepareActorForReturn(AActor* Actor, const FPoolConfig& Config, EPoolReturnReason /*Reason*/)
{
	if (!IsValid(Actor)) return;

	Actor->SetActorHiddenInGame(true);

	if (Config.bDisableCollisionOnReturn)
	{
		Actor->SetActorEnableCollision(false);
	}
	if (Config.bDisableTickOnReturn)
	{
		Actor->SetActorTickEnabled(false);
	}
	if (Config.bResetTransformOnReturn)
	{
		Actor->SetActorLocation(FVector(0.f, 0.f, -100000.f), false, nullptr, ETeleportType::TeleportPhysics);
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// Internal — Grow / Shrink
// ─────────────────────────────────────────────────────────────────────────────

void UUltraPoolSubsystem::GrowPoolData(FUltraPoolData& PoolData, int32 Count)
{
	if (Count <= 0) return;

	// Re-entrancy guard: SpawnActorDeferred calls FinishSpawning, which triggers BeginPlay
	// and can cause the engine to tick. If user Blueprint code in BeginPlay calls Acquire
	// on the same pool (e.g. a spawned actor that immediately spawns more actors), we would
	// enter GrowPoolData recursively, corrupting the Slots array mid-resize. This flag
	// short-circuits the nested Acquire back to AcquireFromPoolData's re-entrant guard path.
	PoolData.bGrowthInProgress = true;

	for (int32 i = 0; i < Count; i++)
	{
		AActor* Actor = SpawnPooledActor(PoolData.Config);
		if (!IsValid(Actor)) break;

		const int32 SlotIndex = PoolData.Slots.Num();

		FUltraPoolSlot Slot;
		Slot.Actor = Actor;
		Slot.Generation = 0;
		Slot.bIsActive = false;

		PoolData.Slots.Add(Slot);
		PoolData.FreeList.Push(SlotIndex, 0);
		PoolData.ActorToSlotIndex.Add(Actor, SlotIndex);
		ActorToPoolID.Add(FObjectKey(Actor), PoolData.PoolID);

		PoolData.Stats.TotalSpawnCount++;
	}

	PoolData.Stats.TotalSize     = PoolData.Slots.Num();
	PoolData.Stats.AvailableCount = PoolData.FreeList.Count();
	PoolData.bGrowthInProgress   = false;
}

void UUltraPoolSubsystem::ShrinkPoolData(FUltraPoolData& PoolData, int32 TargetSize)
{
	// Only destroy dormant actors. Never touch active leases.
	while (PoolData.FreeList.Count() > 0 && PoolData.Slots.Num() > TargetSize)
	{
		const int32 SlotIndex = PoolData.FreeList.Pop();
		if (!PoolData.Slots.IsValidIndex(SlotIndex)) continue;

		FUltraPoolSlot& Slot = PoolData.Slots[SlotIndex];
		if (Slot.bIsActive) continue; // Safety guard

		AActor* Actor = Slot.Actor.Get();
		if (IsValid(Actor))
		{
			PoolData.ActorToSlotIndex.Remove(Actor);
			ActorToPoolID.Remove(FObjectKey(Actor));
			Actor->Destroy();
		}

		// Tombstone the slot: we intentionally leave the slot entry in the Slots array as a
		// null-actor placeholder rather than compacting (RemoveAtSwap). Compacting would
		// renumber every subsequent slot index, instantly invalidating all outstanding
		// FPooledActorHandles (which embed ActorIndex) and corrupting the FreeList entries.
		Slot.Actor = nullptr;
	}

	PoolData.Stats.TotalSize     = PoolData.Slots.Num();
	PoolData.Stats.AvailableCount = PoolData.FreeList.Count();
}

// ─────────────────────────────────────────────────────────────────────────────
// Internal — Lease Expiry
// ─────────────────────────────────────────────────────────────────────────────

void UUltraPoolSubsystem::ScanLeaseExpiry(float CurrentTime)
{
	const UUltraPoolSettings* Settings = GetDefault<UUltraPoolSettings>();
	const bool bLog = Settings && Settings->bLogLeaseExpiry;

	for (auto& Pair : PoolMap)
	{
		FUltraPoolData& PoolData = *Pair.Value;
		if (!PoolData.Config.bAutoReturnOnLeaseExpiry) continue;

		for (int32 i = 0; i < PoolData.Slots.Num(); i++)
		{
			FUltraPoolSlot& Slot = PoolData.Slots[i];
			if (!Slot.bIsActive) continue;
			if (Slot.LeaseExpireTime <= 0.f) continue;
			if (CurrentTime < Slot.LeaseExpireTime) continue;

			if (bLog)
			{
				UE_LOG(LogUltraPool, Log, TEXT("Pool '%s' slot %d: lease expired after %.2fs."),
					*PoolData.Config.PoolTag.ToString(), i,
					CurrentTime - Slot.LeaseIssuedTime);
			}

			ReturnToPoolData(PoolData, i, EPoolReturnReason::LeaseExpired);
		}
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// Internal — Validation & Index helpers
// ─────────────────────────────────────────────────────────────────────────────

bool UUltraPoolSubsystem::ValidateHandleInternal(const FPooledActorHandle& Handle, FUltraPoolData** OutPool) const
{
	if (!Handle.IsValidHandle()) return false;

	const TSharedPtr<FUltraPoolData>* FoundPool = PoolMap.Find(Handle.PoolID);
	if (!FoundPool) return false;

	FUltraPoolData* PoolData = FoundPool->Get();
	if (!PoolData) return false;
	if (!PoolData->Slots.IsValidIndex(Handle.ActorIndex)) return false;

	const FUltraPoolSlot& Slot = PoolData->Slots[Handle.ActorIndex];

	// Core stale-handle detection: every call to ReturnToPoolData increments Slot.Generation.
	// Any handle that was issued for this slot before the last Return will have an older
	// generation number and is immediately detected as stale here — even if the slot has
	// since been re-acquired by a completely different caller.
	if (Slot.Generation != Handle.Generation) return false;

	if (OutPool) *OutPool = PoolData;
	return true;
}

void UUltraPoolSubsystem::RegisterPoolIntoIndex(int32 PoolID, const FPoolConfig& Config)
{
	if (Config.PoolTag.IsNone())
	{
		ClassToPoolID.Add(Config.ActorClass, PoolID);
	}
	else
	{
		TagToPoolID.Add(Config.PoolTag, PoolID);
	}
}

void UUltraPoolSubsystem::UnregisterPoolFromIndex(int32 PoolID, const FPoolConfig& Config)
{
	if (Config.PoolTag.IsNone())
	{
		ClassToPoolID.Remove(Config.ActorClass);
	}
	else
	{
		TagToPoolID.Remove(Config.PoolTag);
	}
}
