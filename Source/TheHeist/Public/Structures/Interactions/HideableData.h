    #pragma once

#include "CoreMinimal.h"
#include "InteractionData.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/PlayerInteractionComponent.h"
#include "HideableData.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class THEHEIST_API UHideableData : public UInteractionData
{
    GENERATED_BODY()

public:

    UHideableData();
        
    //Hidding state
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hideable")
    bool bIsUsed;

    //String for the arrow component used for the hidding position
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hideable")
    FString HiddenArrowName;

    //String for the arrow component used for the quitting position
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hideable")
    FString QuitArrowName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hideable")
    float HideTickRate = 0.02f;
        
    virtual void ExecuteInteraction_Implementation(AActor* Owner, USceneComponent* Target) override;

private:
    
    FTimerHandle HideTimerHandle;
    FVector HiddenLocation;
    FVector QuitLocation;

    TWeakObjectPtr<APawn> PlayerRef;
    TWeakObjectPtr<class UPlayerInteractionComponent> PlayerComp;

    void HideStep(AActor* Owner);
        
};