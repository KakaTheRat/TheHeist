// Fill out your copyright notice in the Description page of Project Settings.


#include "Cailloux.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


ACailloux::ACailloux()
{

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		StaticMesh->SetStaticMesh(SphereMesh.Object);
	}
	
	if(!ProjectileMovementComponent)
	{
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		//ProjectileMovementComponent->SetUpdatedComponent(StaticMesh);
		ProjectileMovementComponent->bAutoActivate = false;
		ProjectileMovementComponent->Deactivate();
		ProjectileMovementComponent->InitialSpeed = 3000.0f;
		ProjectileMovementComponent->MaxSpeed = 3000.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 0.3f;
		ProjectileMovementComponent->ProjectileGravityScale = 1.0f;
		
	}

	if (ObjectDataAsset)
	{
		Cooldown = ObjectDataAsset->ObjectStruct.General.Cooldown;
		if (ObjectDataAsset->ObjectStruct.General.Mesh)
		{
			StaticMesh->SetStaticMesh(ObjectDataAsset->ObjectStruct.General.Mesh);
		}
		else
		{
			
		}
	}
}

void ACailloux::BeginPlay()
{
	Super::BeginPlay();
	if (!StaticMesh)
	{
		if (AObjectBase* Parent = Cast<AObjectBase>(GetAttachParentActor()))
		{
			StaticMesh = Parent->StaticMesh;
		}
		
	}

	if (ProjectileMovementComponent && StaticMesh)
	{
		ProjectileMovementComponent->SetUpdatedComponent(StaticMesh);
	}
}


void ACailloux::UseObject_Implementation()
{
	Super::UseObject_Implementation();
	if(ProjectileMovementComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hello World"));
		ProjectileMovementComponent->Activate(true);
		ProjectileMovementComponent->SetVelocityInLocalSpace(FVector::ForwardVector * LaunchSpeed);
		
	}
	
}

void ACailloux::StartCharging()
{
	/*if (bIsCharging)
	{
		return;
	}*/

	bIsCharging = true;
	ChargeStartTime = GetWorld()->GetTimeSeconds();
	CurrentForce = MinForce;	
}

void ACailloux::StopCharging()
{
	/*if (!bIsCharging)
	{
		return;
	}*/

	bIsCharging = false;
	float HeldTime = GetWorld()->GetTimeSeconds() - ChargeStartTime;
	float ChargeAlpha = FMath::Clamp(HeldTime / ChargeTimeToMax, 0.f, 1.f);
	LaunchSpeed = FMath::Lerp(MinForce, MaxForce, ChargeAlpha);
	UseObject_Implementation();

}


void ACailloux::StartPreview()
{
	if (bIsPreviewing) return;

	bIsPreviewing = true;

	GetWorld()->GetTimerManager().SetTimer(
		PreviewTimerHandle,
		this,
		&ACailloux::UpdatePreview,
		0.03f,
		true
	);
}

void ACailloux::StopPreview()
{
	bIsPreviewing = false;
	GetWorld()->GetTimerManager().ClearTimer(PreviewTimerHandle);
}


void ACailloux::UpdatePreview()
{
	float ForceToUse = bIsCharging ? CurrentForce : MinForce;
	PreviewTrajectory(ForceToUse);
}

void ACailloux::PreviewTrajectory(float Force)
{
	FVector StartLocation = StaticMesh->GetComponentLocation();
	FVector LaunchVelocity = StaticMesh->GetForwardVector() * Force;

	FPredictProjectilePathParams Params;
	Params.StartLocation = StartLocation;
	Params.LaunchVelocity = LaunchVelocity;
	Params.ProjectileRadius = 5.f;
	Params.bTraceWithCollision = true;
	Params.MaxSimTime = 3.f;
	Params.SimFrequency = 15.f;

	FPredictProjectilePathResult Result;
	UGameplayStatics::PredictProjectilePath(this, Params, Result);

	// Draw trajectory lines
	for (int32 i = 0; i < Result.PathData.Num() - 1; i++)
	{
		FVector PointA = Result.PathData[i].Location;
		FVector PointB = Result.PathData[i + 1].Location;

		DrawDebugLine(
			GetWorld(),
			PointA,
			PointB,
			FColor::Green,
			false,      // persistent lines
			0.03f,      // lifetime
			0,
			2.0f       // thickness
		);
	}
}


void ACailloux::ClearSpline()
{
	TrajectorySpline->ClearSplinePoints();
	TrajectorySpline->UpdateSpline();
}
