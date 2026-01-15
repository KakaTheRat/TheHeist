// Fill out your copyright notice in the Description page of Project Settings.


#include "Gadgets/Gadgets.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AGadgets::AGadgets()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);


}

// Called when the game starts or when spawned
void AGadgets::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGadgets::CooldownTimer()
{
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,          
		this,                
		&AGadgets::ChangeCanBeUsed, 
		Cooldown,        
		false              
	);
}
float AGadgets::TakeGadget()
{
	bIsTaking = !bIsTaking;

	if (!DA_Gadget || !DA_Gadget->GadgetStruct.EffectGadget.AnimationGadget)
	{
		return 0.0f;
	}

	if (bIsTaking)
	{
		// On prend le gadget
		return PlayMontage(DA_Gadget->GadgetStruct.EffectGadget.AnimationGadget);
	}
	else
	{
		// On repose le gadget
		return PlayMontage(DA_Gadget->GadgetStruct.EffectGadget.AnimationGadget, -1);
	}
}


void AGadgets::SetNoPhysicObject()
{
	UProjectileMovementComponent* ProjComp = FindComponentByClass<UProjectileMovementComponent>();
	if (ProjComp)
	{
		ProjComp->StopMovementImmediately();
		ProjComp->Deactivate();
		ProjComp->Velocity = FVector::ZeroVector;
	}
	StaticMesh->SetEnableGravity(false);
	StaticMesh->SetSimulatePhysics(false);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


// Called every frame
void AGadgets::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGadgets::OnUsePressed_Implementation()
{
}

void AGadgets::OnUseReleased_Implementation()
{
}


void AGadgets::OnDropPressed_Implementation()
{
}

