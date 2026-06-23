// ============================================================================
// UltraPool - High-Performance Actor Pooling System for Unreal Engine 5
// Author  : Théo de Nanassy
// YouTube : https://www.youtube.com/@UnrealExplorerFR
//
// Copyright (c) 2026 Théo de Nanassy - All rights reserved.
// Distributed via Fab. Unauthorized redistribution is prohibited.
// ============================================================================

#include "UltraPoolComponent.h"
#include "UltraPoolSubsystem.h"

UUltraPoolComponent::UUltraPoolComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UUltraPoolComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UUltraPoolComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// If we still have a valid lease and we're not already returning (guard),
	// return to pool. This handles the case where the owner is destroyed externally.
	if (!bReturnGuard && CurrentHandle.IsValidHandle())
	{
		bReturnGuard = true;

		if (UUltraPoolSubsystem* Subsystem = UUltraPoolSubsystem::Get(this))
		{
			Subsystem->ReturnActor(CurrentHandle);
		}

		CurrentHandle.Invalidate();
	}

	Super::EndPlay(EndPlayReason);
}

void UUltraPoolComponent::ForceReturn()
{
	if (!CurrentHandle.IsValidHandle() || bReturnGuard) return;

	bReturnGuard = true;

	if (UUltraPoolSubsystem* Subsystem = UUltraPoolSubsystem::Get(this))
	{
		Subsystem->ReturnActor(CurrentHandle);
	}

	CurrentHandle.Invalidate();
	bReturnGuard = false;
}

float UUltraPoolComponent::GetTimeRemaining() const
{
	if (!CurrentHandle.IsValidHandle()) return 0.f;

	if (UUltraPoolSubsystem* Subsystem = UUltraPoolSubsystem::Get(this))
	{
		return Subsystem->GetLeaseTimeRemaining(CurrentHandle);
	}
	return 0.f;
}

void UUltraPoolComponent::ExtendLease(float ExtraSeconds)
{
	if (!CurrentHandle.IsValidHandle()) return;

	if (UUltraPoolSubsystem* Subsystem = UUltraPoolSubsystem::Get(this))
	{
		Subsystem->ExtendLease(CurrentHandle, ExtraSeconds);
	}
}

void UUltraPoolComponent::NotifyAcquired(const FPooledActorHandle& Handle)
{
	bReturnGuard = false;
	CurrentHandle = Handle;
	OnAcquired.Broadcast(Handle);
}

void UUltraPoolComponent::NotifyReturned(EPoolReturnReason Reason)
{
	CurrentHandle.Invalidate();
	OnReturned.Broadcast(Reason);
	bReturnGuard = false;
}
