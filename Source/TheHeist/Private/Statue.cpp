// Fill out your copyright notice in the Description page of Project Settings.


#include "Statue.h"
#include "Kismet/GameplayStatics.h"
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

bool AStatue::Raycast()
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

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.f, 0, 5.f);
	
	if (!bHit)
	{
		bIsHitStatue = false;
		UE_LOG(LogTemp, Warning, TEXT("Raycast n'a touché aucun acteur"));
		return false;
	}

	AActor* HitActor = Hit.GetActor();
	if (!IsValid(HitActor))
	{
		bIsHitStatue = false;
		return false;
	}

	bIsHitStatue = HitActor->ActorHasTag("Statue");
	return bIsHitStatue;
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
		
		RaycastAfterRotation();
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
			Light->SetActive(false);
	}
	CurrentLineLights.Empty();

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
	UE_LOG(LogTemp, Warning, TEXT("Raycast n'a touché aucun acteur"));
	if (!bHit)
		return;

	AStatue* OtherStatue = Cast<AStatue>(Hit.GetActor());
	if (!OtherStatue)
		return;

	// Aligned avec une autre statue
	bIsHitStatue = true;

	// Intensité forte si l'autre statue n'est pas disponible
	bool bStrongIntensity = !OtherStatue->GetIsAvailableStatue();

	SpawnLightsBetween(Start, Hit.ImpactPoint, bStrongIntensity);
}

void AStatue::SpawnLightsBetween(const FVector& Start, const FVector& End, bool bStrong)
{
	float Distance = FVector::Distance(Start, End);

	float DynamicSpacing = Distance / FMath::Clamp(FMath::CeilToInt(Distance / 150.f), 1, 50); 
	DynamicSpacing = FMath::Clamp(DynamicSpacing, 100.f, 300.f);

	int32 Count = FMath::FloorToInt(Distance / DynamicSpacing);
	if (Count <= 0) return;

	FVector Direction = (End - Start).GetSafeNormal();

	CurrentLineLights.Empty(); // On commence une nouvelle ligne

	for (int32 i = 1; i <= Count; i++)
	{
		FVector Pos = Start + Direction * DynamicSpacing * i;
		Pos.Z = FMath::Lerp(Start.Z, End.Z, (DynamicSpacing * i) / Distance);

		// Vérifier si un acteur light existe déjà
		TArray<AActor*> FoundLights;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALightPointActor::StaticClass(), FoundLights);

		ALightPointActor* LightActor = nullptr;

		for (AActor* Actor : FoundLights)
		{
			if (FVector::Dist(Actor->GetActorLocation(), Pos) < 50.f)
			{
				LightActor = Cast<ALightPointActor>(Actor);
				break;
			}
		}

		if (LightActor)
		{
			LightActor->SetActive(true);
			LightActor->SetLightIntensity(bStrong ? StrongIntensity : WeakIntensity);
		}
		else
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			LightActor = GetWorld()->SpawnActor<ALightPointActor>(ALightPointActor::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
			if (LightActor)
			{
				LightActor->SetLightIntensity(bStrong ? StrongIntensity : WeakIntensity);
			}
		}

		// Ajouter la light à la ligne courante
		if (LightActor)
		{
			CurrentLineLights.Add(LightActor);
		}
	}
}
