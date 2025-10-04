    #pragma once

#include "CoreMinimal.h"
#include "InteractionData.h"
#include "UObject/NoExportTypes.h"
#include "Player/PlayerInventory.h"
#include "CollectableData.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class THEHEIST_API UCollectableData : public UInteractionData
{
    GENERATED_BODY()

public:

    UCollectableData();
        
    //Collectable quantity 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectable")
    int32 Quantity = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collectable")
    TSubclassOf<class AGadgets> GadgetClass;
        
    virtual void ExecuteInteraction_Implementation(AActor* Owner, USceneComponent* Target) override;
    
};

    
