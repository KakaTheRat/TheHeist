// Fill out your copyright notice in the Description page of Project Settings.


#include "Gadgets/ThrowableGadget.h"

AThrowableGadget::AThrowableGadget()
{
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");	
}
