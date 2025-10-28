    #pragma once

#include "CoreMinimal.h"
#include "InteractionData.h"
#include "UObject/NoExportTypes.h"
#include "Player/PlayerInventory.h"
#include "CollectableData.generated.h"

UCLASS(Blueprintable, EditInlineNew, DisplayName="Collecting Interaction")
class THEHEIST_API UCollectableData : public UInteractionData
{
    GENERATED_BODY()

public:

    //----------Functions
    
    UCollectableData();
    
    virtual void ExecuteInteraction(AActor* Owner, USceneComponent* Target, EInteractionContext Context, AActor* InteractingActor) override;


    //-----------Properties
    
    //Collectable quantity 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectable")
    int32 Quantity = 1;

    //Gadget subclass for the object
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collectable")
    TSubclassOf<class AGadgets> GadgetClass;
        
    
    
};

    
