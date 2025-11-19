    #pragma once

    #include "CoreMinimal.h"
	#include "Helpers/Interactions/InteractionHelpers.h"
    #include "UObject/NoExportTypes.h"
    #include "Perception/AISense_Hearing.h"
    #include "Perception/AISense_Sight.h"
    #include "Perception/AIPerceptionSystem.h"
    #include "Datas/Enumerators/Guard/AlertTypes.h"
#include "Datas/Enumerators/Interactions/InteractionContextEnum.h"
#include "InteractionData.generated.h"

    class UInteractableComponent;
	class UInteractionData;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInteractionEnded, AActor* /*InteractingActor*/, UInteractionData* /*Interaction*/);

    UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
    class THEHEIST_API UInteractionData : public UObject
    {
        GENERATED_BODY()

    public:

    	virtual TArray<FName> GetAvailableStates() ;



    	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    	FName GetCurrentState() const;
    	
    	virtual FName GetCurrentState_Implementation() const { return "None"; }

    	
    	
    	UPROPERTY(VisibleAnywhere)
    	AActor* OwnerActor;

    	void SetActor(AActor* Acptr)
    	{
    		OwnerActor = Acptr;
    	}

    	AActor* GetActor(){return OwnerActor;};
        //--------------Functions

        //End of interaction event. Must be called whenever an interaction is over
        FOnInteractionEnded OnInteractionEnded;

    	//Set up the end sequence of each interaction
    	UFUNCTION(BlueprintCallable,Category="Interaction")
    	void EndOfInteraction();

        //Interaction execution. Must be overrided by each interaction type
        virtual void ExecuteInteraction(AActor* m_Owner, USceneComponent* m_Target, EInteractionContext m_Context, AActor* m_InteractingActor);

    	virtual void StartInteraction();
    	
        //Virtual function, to tick the UObject into the interaction component. Override this to make is happen
        virtual void Tick(float DeltaTime) {}

        
        //--------------Properties

        // Displayed text on the interaction widget
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
        FString InteractText;

    	UPROPERTY(EditAnywhere, Category="Interaction")
    	bool bUseExternalActor = false;

    	// External actor 
    	UPROPERTY(EditAnywhere, Category="Interaction", meta=(EditCondition="bUseExternalActor"))
    	AActor* ExternalActor = nullptr;

    	//Dropdown of available components for the owner or given actor
    	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction", meta = (GetOptions = "GetAvailableComponents"))
    	FName CompNames;

    	//Bool to determines if this interaction should be seen and used by the player
    	UPROPERTY(EditAnywhere, Category="Interaction")
		bool bShouldAppearForThePlayer = true;
    	
    	
    	
    	UFUNCTION()
    	
    	TArray<FName> GetAvailableComponents() const
    	{
    		/*UE_LOG(LogTemp, Warning, TEXT("HIHIHIHIH"));
    		TArray<FName> Result;

    		if (!OwnerActor) return Result;

    		TArray<USceneComponent*> Components;
    		OwnerActor->GetComponents<USceneComponent>(Components);

    		for (USceneComponent* Comp : Components)
    		{
    			if (Comp)
    			{
    				UE_LOG(LogTemp, Warning, TEXT("%s"), *Comp->GetName());
    				Result.Add(Comp->GetFName());
    			}
    		}

    		return Result;*/
    		TArray<FName> Result;

    		// Détermine sur quel acteur on travaille
    		AActor* TargetActor = nullptr;
    		if (bUseExternalActor && ExternalActor)
    			TargetActor = ExternalActor;
    		else
    			TargetActor = OwnerActor;

    		if (!TargetActor)
    			return Result;

    		// Liste les composants
    		TArray<USceneComponent*> Components;
    		TargetActor->GetComponents<USceneComponent>(Components);

    		for (USceneComponent* Comp : Components)
    		{
    			if (Comp)
    			{
    				Result.Add(Comp->GetFName());
    			}
    		}

    		return Result;
    	}






    	// Animation montage à jouer avant d'exécuter l'interaction
    	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction|Animation")
    	UAnimMontage* InteractionMontage = nullptr;

    	// Nom de l'AnimNotify attendu (ex: "OnInteract")
    	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction|Animation")
    	FName MontageNotifyToTrigger = "OnInteract";

    	// Flag pour savoir si on est en attente du notify
    	bool bWaitingForAnimation = true;

    	UFUNCTION()
    	void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

    	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction|Animation")
    	float PlayRate = 1;
    	
    	UPROPERTY(editAnywhere, BlueprintReadWrite, Category="Interaction|Animation", meta = (GetOptions = "GetAvailableComponents"))
    	FName AnimationTarget;

    	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction|Animation")
    	FName BoneTarget = EName::None;
    	

    	UPROPERTY()
    	AActor* Owner;

    	UPROPERTY()
    	USceneComponent* Target;

    	UPROPERTY()
    	EInteractionContext Context;

    	UPROPERTY()
    	AActor* InteractingActor;



    	
       
#pragma region Alert
        //-------------ALERT

        //--------------Alert Properties
        
        //Defines if the interaction can or cannot have an effect on the guard's behavior
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
        bool bCanAlertGuards = false;

        //Type of alert generated by the interaction
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert", meta = (EditCondition = "bCanAlertGuards"))
        EAlertTypes AlertType;

        // Range of the alert in units
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert", meta = (EditCondition = "bCanAlertGuards"))
        float AlertRadius = 500.f;

        // Loudness of the sound (only for Auditive alerts)
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert", meta = (EditCondition = "bCanAlertGuards && AlertType==EAlertType::Auditive"))
        float Loudness = 1.0f;

        
        UPROPERTY()
        class UAIPerceptionStimuliSourceComponent* StimulusSource;
        
        //-------------Alert Functions
        
        // Function to trigger guard's alert
        UFUNCTION(BlueprintCallable)
        void TriggerAlert(AActor* SourceActor,TSubclassOf<UAISense> Sense);
        
        // Function to clear guard's alert
        UFUNCTION(BlueprintCallable)
        void ClearAlert(AActor* SourceActor, TSubclassOf<UAISense> Sense);

#pragma endregion
        
        protected:
        
        //Hit Component by the user's interaction   
        UPROPERTY()
        USceneComponent* LinkedComponent = nullptr;


    	UPROPERTY(EditAnywhere)
    	AActor* CurrentInteractingActor;
    	

    	virtual void PostInitProperties() override;

    	UFUNCTION()
    	void PlayAnimation();
    	
    };

    

    


    

    

    