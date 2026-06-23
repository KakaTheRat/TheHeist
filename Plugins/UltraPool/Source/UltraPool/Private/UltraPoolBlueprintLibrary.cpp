// ============================================================================
// UltraPool - High-Performance Actor Pooling System for Unreal Engine 5
// Author  : Théo de Nanassy
// YouTube : https://www.youtube.com/@UnrealExplorerFR
//
// Copyright (c) 2026 Théo de Nanassy - All rights reserved.
// Distributed via Fab. Unauthorized redistribution is prohibited.
// ============================================================================

#include "UltraPoolBlueprintLibrary.h"
#include "UltraPoolSubsystem.h"
#include "GameFramework/Actor.h"

// ─────────────────────────────────────────────────────────────────────────────
// Internal helper
// ─────────────────────────────────────────────────────────────────────────────

UUltraPoolSubsystem* UUltraPoolBlueprintLibrary::GetSubsystem(const UObject* WorldContextObject)
{
	return UUltraPoolSubsystem::Get(WorldContextObject);
}

// ─────────────────────────────────────────────────────────────────────────────
// Registration
// ─────────────────────────────────────────────────────────────────────────────

int32 UUltraPoolBlueprintLibrary::UltraPool_RegisterPool(const UObject* WorldContextObject, FPoolConfig Config)
{
	if (UUltraPoolSubsystem* Sub = GetSubsystem(WorldContextObject))
	{
		return Sub->RegisterPool(Config);
	}
	return -1;
}

void UUltraPoolBlueprintLibrary::UltraPool_DeregisterPool(const UObject* WorldContextObject, int32 PoolID, bool bDestroyActors)
{
	if (UUltraPoolSubsystem* Sub = GetSubsystem(WorldContextObject))
	{
		Sub->DeregisterPool(PoolID, bDestroyActors);
	}
}

void UUltraPoolBlueprintLibrary::UltraPool_WarmUp(const UObject* WorldContextObject, int32 PoolID)
{
	if (UUltraPoolSubsystem* Sub = GetSubsystem(WorldContextObject))
	{
		Sub->WarmUpPool(PoolID);
	}
}

void UUltraPoolBlueprintLibrary::UltraPool_WarmUpAll(const UObject* WorldContextObject)
{
	if (UUltraPoolSubsystem* Sub = GetSubsystem(WorldContextObject))
	{
		Sub->WarmUpAllPools();
	}
}

bool UUltraPoolBlueprintLibrary::UltraPool_IsRegistered(const UObject* WorldContextObject, int32 PoolID)
{
	if (UUltraPoolSubsystem* Sub = GetSubsystem(WorldContextObject))
	{
		return Sub->IsPoolRegistered(PoolID);
	}
	return false;
}

// ─────────────────────────────────────────────────────────────────────────────
// Acquire
// ─────────────────────────────────────────────────────────────────────────────

FPooledActorHandle UUltraPoolBlueprintLibrary::UltraPool_Acquire(
	const UObject* WorldContextObject,
	int32 PoolID,
	const FTransform& SpawnTransform,
	bool& bSuccess,
	AActor*& OutActor)
{
	bSuccess = false;
	OutActor = nullptr;

	UUltraPoolSubsystem* Sub = GetSubsystem(WorldContextObject);
	if (!Sub) return FPooledActorHandle{};

	FPooledActorHandle Handle = Sub->AcquireActor(PoolID, SpawnTransform);
	if (Handle.IsValidHandle())
	{
		OutActor = Sub->GetActorFromHandle(Handle);
		bSuccess = IsValid(OutActor);
	}
	return Handle;
}

FPooledActorHandle UUltraPoolBlueprintLibrary::UltraPool_AcquireByClass(
	const UObject* WorldContextObject,
	TSubclassOf<AActor> ActorClass,
	const FTransform& SpawnTransform,
	bool& bSuccess,
	AActor*& OutActor)
{
	bSuccess = false;
	OutActor = nullptr;

	UUltraPoolSubsystem* Sub = GetSubsystem(WorldContextObject);
	if (!Sub) return FPooledActorHandle{};

	FPooledActorHandle Handle = Sub->AcquireActorByClass(ActorClass, SpawnTransform);
	if (Handle.IsValidHandle())
	{
		OutActor = Sub->GetActorFromHandle(Handle);
		bSuccess = IsValid(OutActor);
	}
	return Handle;
}

FPooledActorHandle UUltraPoolBlueprintLibrary::UltraPool_AcquireByTag(
	const UObject* WorldContextObject,
	FName PoolTag,
	const FTransform& SpawnTransform,
	bool& bSuccess,
	AActor*& OutActor)
{
	bSuccess = false;
	OutActor = nullptr;

	UUltraPoolSubsystem* Sub = GetSubsystem(WorldContextObject);
	if (!Sub) return FPooledActorHandle{};

	FPooledActorHandle Handle = Sub->AcquireActorByTag(PoolTag, SpawnTransform);
	if (Handle.IsValidHandle())
	{
		OutActor = Sub->GetActorFromHandle(Handle);
		bSuccess = IsValid(OutActor);
	}
	return Handle;
}

// ─────────────────────────────────────────────────────────────────────────────
// Return
// ─────────────────────────────────────────────────────────────────────────────

void UUltraPoolBlueprintLibrary::UltraPool_Return(const UObject* WorldContextObject, FPooledActorHandle Handle)
{
	if (UUltraPoolSubsystem* Sub = GetSubsystem(WorldContextObject))
	{
		Sub->ReturnActor(Handle);
	}
}

void UUltraPoolBlueprintLibrary::UltraPool_ReturnByActor(const UObject* WorldContextObject, AActor* Actor)
{
	if (UUltraPoolSubsystem* Sub = GetSubsystem(WorldContextObject))
	{
		Sub->ReturnActorByPointer(Actor);
	}
}

void UUltraPoolBlueprintLibrary::UltraPool_ReturnAll(const UObject* WorldContextObject, int32 PoolID)
{
	if (UUltraPoolSubsystem* Sub = GetSubsystem(WorldContextObject))
	{
		Sub->ReturnAllActors(PoolID);
	}
}

void UUltraPoolBlueprintLibrary::UltraPool_ExtendLease(const UObject* WorldContextObject, FPooledActorHandle Handle, float ExtraSeconds)
{
	if (UUltraPoolSubsystem* Sub = GetSubsystem(WorldContextObject))
	{
		Sub->ExtendLease(Handle, ExtraSeconds);
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// Query
// ─────────────────────────────────────────────────────────────────────────────

bool UUltraPoolBlueprintLibrary::UltraPool_IsHandleValid(FPooledActorHandle Handle)
{
	return Handle.IsValidHandle();
}

AActor* UUltraPoolBlueprintLibrary::UltraPool_GetActor(const UObject* WorldContextObject, FPooledActorHandle Handle)
{
	if (UUltraPoolSubsystem* Sub = GetSubsystem(WorldContextObject))
	{
		return Sub->GetActorFromHandle(Handle);
	}
	return nullptr;
}

FPoolRuntimeStats UUltraPoolBlueprintLibrary::UltraPool_GetPoolStats(const UObject* WorldContextObject, int32 PoolID)
{
	if (UUltraPoolSubsystem* Sub = GetSubsystem(WorldContextObject))
	{
		return Sub->GetPoolStats(PoolID);
	}
	return FPoolRuntimeStats{};
}

FGlobalPoolStats UUltraPoolBlueprintLibrary::UltraPool_GetGlobalStats(const UObject* WorldContextObject)
{
	if (UUltraPoolSubsystem* Sub = GetSubsystem(WorldContextObject))
	{
		return Sub->GetGlobalStats();
	}
	return FGlobalPoolStats{};
}

float UUltraPoolBlueprintLibrary::UltraPool_GetTimeRemaining(const UObject* WorldContextObject, FPooledActorHandle Handle)
{
	if (UUltraPoolSubsystem* Sub = GetSubsystem(WorldContextObject))
	{
		return Sub->GetLeaseTimeRemaining(Handle);
	}
	return 0.f;
}

// ─────────────────────────────────────────────────────────────────────────────
// Utility
// ─────────────────────────────────────────────────────────────────────────────

FPoolConfig UUltraPoolBlueprintLibrary::UltraPool_MakeConfig(
	TSubclassOf<AActor> ActorClass,
	int32 InitialSize,
	int32 MaxSize,
	float LeaseDuration,
	EPoolGrowthPolicy GrowthPolicy,
	FName Tag)
{
	FPoolConfig Config;
	Config.ActorClass            = ActorClass;
	Config.InitialSize           = FMath::Max(0, InitialSize);
	Config.MaxSize               = FMath::Max(0, MaxSize);
	Config.DefaultLeaseDuration  = FMath::Max(0.f, LeaseDuration);
	Config.GrowthPolicy          = GrowthPolicy;
	Config.PoolTag               = Tag;
	return Config;
}

int32 UUltraPoolBlueprintLibrary::UltraPool_ComputePoolID(TSubclassOf<AActor> ActorClass, FName Tag)
{
	return UUltraPoolSubsystem::ComputePoolID(ActorClass, Tag);
}

FString UUltraPoolBlueprintLibrary::UltraPool_HandleToString(FPooledActorHandle Handle)
{
	return Handle.ToString();
}

void UUltraPoolBlueprintLibrary::UltraPool_TrimAll(const UObject* WorldContextObject)
{
	if (UUltraPoolSubsystem* Sub = GetSubsystem(WorldContextObject))
	{
		Sub->TrimAllIdlePools();
	}
}

void UUltraPoolBlueprintLibrary::UltraPool_ForceGrow(const UObject* WorldContextObject, int32 PoolID, int32 Count)
{
	if (UUltraPoolSubsystem* Sub = GetSubsystem(WorldContextObject))
	{
		Sub->GrowPool(PoolID, Count);
	}
}
