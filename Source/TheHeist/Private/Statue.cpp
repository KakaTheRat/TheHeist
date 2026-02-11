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
	UE_LOG(LogTemp, Warning, TEXT("Raycast() CALLED"));

	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * 10000.f;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FHitResult Hit;
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	if (!bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Raycast: NO HIT"));
		return false;
	}

	AActor* HitActor = Hit.GetActor();
	if (!HitActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Raycast: HIT but no actor"));
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("Raycast HIT actor: %s"), *HitActor->GetName());

	bool bIsStatue = HitActor->ActorHasTag("Statue");

	UE_LOG(LogTemp, Warning, TEXT("Has tag Statue: %s"), bIsStatue ? TEXT("TRUE") : TEXT("FALSE"));

	LastRaycastHit = Hit;

	return bIsStatue;
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
		UE_LOG(LogTemp, Warning, TEXT("=== ROTATION FINIE ==="));

		GetWorld()->GetTimerManager().ClearTimer(RotationTimerHandle);

		bool bHitStatue = Raycast();

		UE_LOG(LogTemp, Warning, TEXT("Raycast returned: %s"), bHitStatue ? TEXT("TRUE") : TEXT("FALSE"));

		if (bHitStatue)
		{
			UE_LOG(LogTemp, Warning, TEXT("Calling SpawnLightsBetween"));

			SpawnLightsBetween(
				GetActorLocation(),
				LastRaycastHit.ImpactPoint,
				false
			);
		}
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
	UE_LOG(LogTemp, Warning, TEXT("Raycast n'a touché  acteur"));
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
	UKismetSystemLibrary::PrintString(
		this,
		FString::Printf(TEXT("SpawnLightsBetween CALLED | Start: %s | End: %s"), 
			*Start.ToString(), *End.ToString()),
		true, true, FColor::Green, 2.f
	);

	float Distance = FVector::Distance(Start, End);

	UKismetSystemLibrary::PrintString(
		this,
		FString::Printf(TEXT("Distance = %.2f"), Distance),
		true, true, FColor::Yellow, 2.f
	);

	float DynamicSpacing = Distance / FMath::Clamp(FMath::CeilToInt(Distance / 150.f), 1, 50);
	DynamicSpacing = FMath::Clamp(DynamicSpacing, 100.f, 300.f);

	int32 Count = FMath::FloorToInt(Distance / DynamicSpacing);

	UKismetSystemLibrary::PrintString(
		this,
		FString::Printf(TEXT("Spacing = %.2f | Count = %d"), DynamicSpacing, Count),
		true, true, FColor::Cyan, 2.f
	);

	if (Count <= 0)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("COUNT <= 0 -> EXIT"), true, true, FColor::Red, 2.f);
		return;
	}

	FVector Direction = (End - Start).GetSafeNormal();

	CurrentLineLights.Empty();

	for (int32 i = 1; i <= Count; i++)
	{
		FVector Pos = Start + Direction * DynamicSpacing * i;
		Pos.Z = 200;

		UKismetSystemLibrary::PrintString(
			this,
			FString::Printf(TEXT("Light %d Position: %s"), i, *Pos.ToString()),
			true, true, FColor::White, 1.f
		);

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
			UKismetSystemLibrary::PrintString(this, TEXT("Existing light found"), true, true, FColor::Blue, 1.f);

			LightActor->SetActive(true);
			LightActor->SetLightIntensity(bStrong ? StrongIntensity : WeakIntensity);
		}
		else
		{
			UKismetSystemLibrary::PrintString(this, TEXT("Spawning NEW light"), true, true, FColor::Orange, 1.f);

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;

			LightActor = GetWorld()->SpawnActor<ALightPointActor>(
				LightSub,
				Pos,
				FRotator::ZeroRotator,
				SpawnParams
			);

			if (LightActor)
			{
				LightActor->SetLightIntensity(bStrong ? StrongIntensity : WeakIntensity);
			}
			else
			{
				UKismetSystemLibrary::PrintString(this, TEXT("SPAWN FAILED"), true, true, FColor::Red, 2.f);
			}
		}

		if (LightActor)
		{
			CurrentLineLights.Add(LightActor);
		}
	}
}
