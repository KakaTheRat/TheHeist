    #pragma once

#include "CoreMinimal.h"
#include "InteractionData.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/PlayerInteractionComponent.h"
#include "HideableData.generated.h"

UCLASS(Blueprintable, EditInlineNew, DisplayName="Hiding Interaction")
class THEHEIST_API UHideableData : public UInteractionData
{
    GENERATED_BODY()

public:

    //-------------Functions
    
    UHideableData();

    virtual void ExecuteInteraction(AActor* Owner, USceneComponent* Target) override;


    //--------------Properties
    
    //Hidding state (is a user currently hiding
    UPROPERTY(BlueprintReadWrite)
    bool bIsUsed;

    //String for the arrow component used for the hidding position
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hiding")
    FString HiddenArrowName;

    //String for the arrow component used for the quitting position
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hiding")
    FString QuitArrowName;

    //Tick rate for the hiding movement (used for the set timer, less means more fluid)
    float HideTickRate = 0.02f;
        
    

private:


    //---------------Properties
    
    //Timer 
    FTimerHandle HideTimerHandle;
    
    FVector HiddenLocation;
    FVector QuitLocation;


    //Weak reference to the player
    TWeakObjectPtr<APawn> PlayerRef;
    TWeakObjectPtr<class UPlayerInteractionComponent> PlayerComp;


    //----------------Functions

    
    //Interpolates the player's position over time
    void HideStep(AActor* Owner);
        
};