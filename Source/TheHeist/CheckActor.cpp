// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckActor.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ACheckActor::ACheckActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACheckActor::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

// Called every frame
void ACheckActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DoRaycast();

}

void ACheckActor::DoRaycast()
{
	if (bHasDetectedPlayer || !PlayerCharacter) return;

	FVector Start = GetActorLocation() + RayOffset;
	FVector Forward = GetActorForwardVector();

	for (int32 i = 0; i < NumRays; ++i)
	{
		float AngleStep = ConeAngle / NumRays;
		float CurrentAngle = -ConeAngle / 2.f + i * AngleStep;

		FVector RotatedDir = Forward.RotateAngleAxis(CurrentAngle, FVector::UpVector);
		FVector End = Start + RotatedDir * RayLength;

		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.f, 0, 1.f);

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Pawn, Params);

		if (bHit && HitResult.GetActor() == PlayerCharacter)
		{
			if (!bHasDetectedPlayer)
			{
				bHasDetectedPlayer = true;
				OnFindPlayer.Broadcast();
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Le joueur"));
				}
			}
			break;
		}
	}
}

