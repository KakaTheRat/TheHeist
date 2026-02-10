// Fill out your copyright notice in the Description page of Project Settings.


#include "Statue.h"

#include "Math/UnitConversion.h"

// Sets default values
AStatue::AStatue()
{
	PrimaryActorTick.bCanEverTick = true;
	CurrentBaseRotation = GetActorRotation();
	TargetRotation = CurrentBaseRotation;

}

// Called when the game starts or when spawned
void AStatue::BeginPlay()
{
	Super::BeginPlay();
	StartRotation = GetActorRotation();
	TargetRotation = StartRotation;
}

// Called every frame
void AStatue::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AStatue::VerrifyAngle()
{
	if (Angles.IsEmpty())
	{
		return false;
	}
	
	for (auto Angle : Angles)
	{
		if (Angle == Rotation.Yaw)
		{
			return true;
		}
	}
	return false;
}

bool AStatue::Raycast()
{
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * 10000;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	if (!bHit)
	{
		return false;
	}

	AActor* HitActor = Hit.GetActor();
	if (!IsValid(HitActor))
	{
		return false;
	}

	return HitActor->ActorHasTag("Statue");
}

void AStatue::UpdateRotation()
{
	ElapsedTime += 0.01f;

	float Alpha = ElapsedTime / RotationDuration;
	Alpha = FMath::Clamp(Alpha, 0.f, 1.f);

	FRotator NewRot = FMath::Lerp(StartRotation, TargetRotation, Alpha);
	NewRot.Yaw = FMath::Fmod(NewRot.Yaw, 360.f);
	if (NewRot.Yaw < 0.f)
		NewRot.Yaw += 360.f;

	SetActorRotation(NewRot);

	if (Alpha >= 1.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(RotationTimerHandle);
	}
}

void AStatue::RotateRight()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(RotationTimerHandle))
	{
		return;
	}

	StartRotation = GetActorRotation();
	TargetRotation.Yaw = StartRotation.Yaw + 90.f;
	TargetRotation.Yaw = FMath::Fmod(TargetRotation.Yaw, 360.f);
	if (TargetRotation.Yaw < 0.f)
		TargetRotation.Yaw += 360.f;

	ElapsedTime = 0.f;

	GetWorld()->GetTimerManager().SetTimer(
		RotationTimerHandle,
		this,
		&AStatue::UpdateRotation,
		0.01f, true
	);
}

/*void AStatue::Rotate()
{
	Rotation = InitalRotator + FRotator(0,45,0);
	if (InitalRotator.Yaw == 360)
	{
		InitalRotator.Yaw = 0;
	}
	SetActorRotation(Rotation);
	
	UE_LOG(LogTemp, Warning, TEXT("Hello Unreal"));
}*/



/*void AStatue::SetNextRotationStep()
{
	CurrentBaseRotation.Yaw += StepAngle;

	if (CurrentBaseRotation.Yaw >= 360.f)
		CurrentBaseRotation.Yaw -= 360.f;

	TargetRotation = CurrentBaseRotation;

	*UE_LOG(LogTemp, Warning, TEXT("New target yaw: %.1f"), TargetRotation.Yaw);
}*/