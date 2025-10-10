#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Guard.generated.h"

UCLASS()
class THEHEIST_API AGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGuard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Fonction appelée quand la caméra alerte le garde */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "AI")
	void OnAlerted(AActor* DetectedPlayer);

	/** Retourne la position du garde */
	UFUNCTION(BlueprintCallable, Category = "AI")
	FVector GetGuardLocation() const;
};
