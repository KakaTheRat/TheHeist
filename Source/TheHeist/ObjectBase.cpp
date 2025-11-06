// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectBase.h"
#include "GameFramework/Character.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

AObjectBase::AObjectBase()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereComponent"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		StaticMesh->SetStaticMesh(SphereMesh.Object);
	}
	RootComponent = StaticMesh;

}

// Called when the game starts or when spawned
void AObjectBase::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	
}

// Called every frame
void AObjectBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AObjectBase::PlayAnimation(UAnimMontage* Anim)
{
	if (!Anim || !OwnerCharacter) return;

	// Si on joue déjà cette anim → inutile de relancer
	if (CurrentAnim == Anim) return;

	OwnerCharacter->PlayAnimMontage(Anim);
	CurrentAnim = Anim;
}

void AObjectBase::StopAnimation()
{
	if (!OwnerCharacter || !CurrentAnim)
	{
		return;
	}

	OwnerCharacter->StopAnimMontage(CurrentAnim);
	CurrentAnim = nullptr;
}

void AObjectBase::PlayNiagara(UNiagaraSystem* NiagaraSystem)
{
	if (!NiagaraSystem)
	{
		return;
	}

	if (!NiagaraComp)
	{
		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
			NiagaraSystem,
			GetOwner()->GetRootComponent(),
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			false
		);
	}
	else
	{
		NiagaraComp->SetAsset(NiagaraSystem);
		NiagaraComp->Activate();
	}
}

void AObjectBase::StopNiagara()
{
	if (NiagaraComp)
	{
		NiagaraComp->Deactivate();
	}
}

void AObjectBase::PlayAudio(USoundBase* Sound)
{
	if (!Sound)
	{
		return;
	}
	if (!AudioComp)
	{
		AudioComp = UGameplayStatics::SpawnSoundAttached(
			Sound,
			GetOwner()->GetRootComponent()
		);
	}
	else
	{
		if (AudioComp->IsPlaying()) return; // évite redémarrage
		AudioComp->SetSound(Sound);
		AudioComp->Play();
	}
}

void AObjectBase::StopAudio()
{
	if (AudioComp && AudioComp->IsPlaying())
	{
		AudioComp->Stop();
	}
}

void AObjectBase::ExistObject()
{
	if (ObjectDataAsset)
	{
		if (ObjectDataAsset->ObjectStruct.Animation.ExistObjectAnimation)
		{
			PlayAnimation(ObjectDataAsset->ObjectStruct.Animation.ExistObjectAnimation);
		}
		if (ObjectDataAsset->ObjectStruct.Effect.ExitObjectNiagara )
		{
			PlayNiagara(ObjectDataAsset->ObjectStruct.Effect.ExitObjectNiagara);
		}
		if (ObjectDataAsset->ObjectStruct.Effect.ExitObjectSound)
		{
			PlayAudio(ObjectDataAsset->ObjectStruct.Effect.ExitObjectSound);
		}
	}
}


void AObjectBase::UseObject_Implementation()
{
	
}

void AObjectBase::ReturnObject()
{
	if (ObjectDataAsset)
	{
		if (ObjectDataAsset->ObjectStruct.Animation.ReturnObjectAnimation)
		{
			PlayAnimation(ObjectDataAsset->ObjectStruct.Animation.ReturnObjectAnimation);
		}
		if (ObjectDataAsset->ObjectStruct.Effect.ReturnObjectNiagara )
		{
			PlayNiagara(ObjectDataAsset->ObjectStruct.Effect.ReturnObjectNiagara);
		}
		if (ObjectDataAsset->ObjectStruct.Effect.ReturnObjectSound)
		{
			PlayAudio(ObjectDataAsset->ObjectStruct.Effect.ReturnObjectSound);
		}
	}
}
