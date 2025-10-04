    #pragma once

    #include "CoreMinimal.h"
    #include "UObject/NoExportTypes.h"
    #include "InteractionData.generated.h"

    class UInteractableComponent;

    UCLASS(Abstract, Blueprintable, EditInlineNew)
    class THEHEIST_API UInteractionData : public UObject
    {
        GENERATED_BODY()

    public:

        // Displayed text on the interaction widget
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
        FString InteractText;

        //Interaction execution. Overrided by each interaction type
        UFUNCTION(BlueprintNativeEvent, Category="Interaction")
        void ExecuteInteraction(AActor* Owner, USceneComponent* Target);

        //Virtual function, to tick the UObject into the interaction component. Override this to make is happen
        virtual void Tick(float DeltaTime) {}
        
        protected:

        //Hit Component by the user's interaction   
        UPROPERTY()
        USceneComponent* LinkedComponent = nullptr;

        
    };

    inline void UInteractionData::ExecuteInteraction_Implementation(AActor* Owner, USceneComponent* Target)
    {
    }

    


    

    

    