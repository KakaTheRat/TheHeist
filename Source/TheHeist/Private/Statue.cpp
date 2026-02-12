// Fill out your copyright notice in the Description page of Project Settings.


#include "Statue.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Components/WidgetComponent.h"
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
	bIsGreateAngle = false;

	for (auto Angle : Angles)
	{
		if (FMath::IsNearlyEqual(Angle, GetActorRotation().Yaw, 0.1f))
		{
			bIsGreateAngle = true;
			break;
		}
	}

	return bIsGreateAngle;
}

void AStatue::Raycast()
{
	FVector Start = GetActorLocation() + FVector(0, 0, 200);
	FVector End = Start + (GetActorForwardVector() * 1000.f);

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

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.f);

	if (bHit && Hit.GetActor())
	{
		HitStatue = Cast<AStatue>(Hit.GetActor());

		if (HitStatue)
		{
			SpawnLightsBetween(
				GetActorLocation(),
				Hit.ImpactPoint,
				HitStatue->GetIsAvailableStatue()
			);
		}
	}
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
		UE_LOG(LogTemp, Warning, TEXT("Je passe dedans"));
		GetWorld()->GetTimerManager().ClearTimer(RotationTimerHandle);
		
		GetWorld()->GetTimerManager().SetTimer(
		RaycastTimerHandle,
		this,
		&AStatue::Raycast,
		0.2f,
		false
	);
	}
}

void AStatue::RotateRight()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(RotationTimerHandle))
	{
		return;
	}
	
	for (ALightPointActor* Light : CurrentLineLights)
	{
		if (Light)
		{
			Light->SetActive(false);
		}
	}
	CurrentLineLights.Empty();

	HitStatue = nullptr;
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


void AStatue::RaycastAfterRotation()
{
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * 10000.f;

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
	
	bIsHitStatue = false;
	UE_LOG(LogTemp, Warning, TEXT("Raycast n'a touché  acteur"));
	if (!bHit)
	{
		return;
	}

	AStatue* OtherStatue = Cast<AStatue>(Hit.GetActor());
	if (!OtherStatue)
	{
		return;
	}

	bIsHitStatue = true;

	bool bStrongIntensity = !OtherStatue->GetIsAvailableStatue();

	SpawnLightsBetween(Start, Hit.ImpactPoint, HitStatue->GetIsAvailableStatue());
}

void AStatue::SpawnLightsBetween(const FVector& Start, const FVector& End, bool bStrong)
{
	const float Distance = FVector::Distance(Start, End);
	if (Distance <= KINDA_SMALL_NUMBER)
		return;

	const float TargetSpacing = 150.f;
	const int32 MaxLights = 50;

	const int32 Count = FMath::Clamp(
		FMath::CeilToInt(Distance / TargetSpacing),
		1,
		MaxLights
	);

	const float Spacing = Distance / Count;
	const FVector Direction = (End - Start).GetSafeNormal();

	CurrentLineLights.Empty();

	// Cache existing lights ONCE
	TArray<AActor*> ExistingLights;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		ALightPointActor::StaticClass(),
		ExistingLights
	);

	for (int32 i = 1; i <= Count; ++i)
	{
		FVector Position = Start + Direction * Spacing * i;
		Position.Z = GetActorLocation().Z + 300.f;

		ALightPointActor* LightActor = FindClosestLight(Position, ExistingLights);

		if (!LightActor)
		{
			FActorSpawnParameters Params;
			Params.Owner = this;

			LightActor = GetWorld()->SpawnActor<ALightPointActor>(
				LightSub,
				Position,
				FRotator::ZeroRotator,
				Params
			);
		}

		if (!LightActor)
		{
			continue;
		}

		LightActor->SetActive(true);
		LightActor->SetLightIntensity(bStrong ? StrongIntensity : WeakIntensity);

		CurrentLineLights.Add(LightActor);
	}
}

ALightPointActor* AStatue::FindClosestLight( const FVector& Position,const TArray<AActor*>& Lights)
{
	const float SearchRadius = 50.f;

	for (AActor* Actor : Lights)
	{
		if (FVector::DistSquared(Actor->GetActorLocation(), Position) < FMath::Square(SearchRadius))
		{
			return Cast<ALightPointActor>(Actor);
		}
	}

	return nullptr;
}