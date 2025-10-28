// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionInterface.h"
#include "Components/ActorComponent.h"
#include "Player/PlayerInteractionComponent.h"
#include "../Interactions/InteractionTypes/InteractionData.h"
#include "Widget/Interaction/InteractionWidgetActor.h"
#include "InteractableComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionEndedEvent, AActor*, InteractingActor, UInteractionData*, InteractionType);

#pragma region InteractionCascadeSlot

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)



//This is used as a slot for each interaction in a cascade. This will allow the setup of each interaction individually, helping the communication with the cascade
class UInteractionCascadeSlot : public UObject
{
	GENERATED_BODY()

public:

	// PROPERTIES //
	
	// Display in the editor the component linked with data
	UPROPERTY(EditAnywhere,  meta=(GetOptions="GetAvailableInteractionComponents"))
	FName SelectedComponentName;

	// Display in the editor the datas linked to the chosen component
	UPROPERTY(EditAnywhere, meta=(GetOptions="GetAvailableInteractionsForSelectedComponent"))
	FName SelectedInteractionName;
	
	//Reference to an interaction 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UInteractionData> InteractionData;

	//Shows the exepected state for the interaction at the end of the interaction. For exemple, if a door must be opened in a cascade, if it's already AND if expected state is set to "Open", then will skip this interaction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(GetOptions="GetAvailableStates"))
	FName ExpectedState;

	UPROPERTY(EditAnywhere)
	bool bShouldInterruptCascade = false;

	// FUNCTIONS //
	
	
	// Returns an array of the available states, depending of the interaction
	UFUNCTION()
	TArray<FName> GetAvailableStates() const;

	// Functions called by the inspector dynamic selection
	UFUNCTION()
	TArray<FName> GetAvailableInteractionComponents();

	// Functions called by the inspector dynamic selection. Gets an array of interactions names
	UFUNCTION()
	TArray<FName> GetAvailableInteractionsForSelectedComponent();
	
	void RefreshData();

	
};

#pragma endregion

#pragma region CascadesDatasStruct

//Structure used for the interaction cascade management. Each cascade has an array of single interaction
USTRUCT(BlueprintType)
struct FInteractionCascadeData
{
	GENERATED_BODY()
	
	//Name to recognize a particular cascade struct
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CascadeName;

	//Array of single interactions matched with a component name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TArray<UInteractionCascadeSlot*> InteractionCascades;

	//Index of the interaction triggering all the cascade.
	//For exemple, the main interaction for hiding in a closet would be the hiding interaction as it triggers everything (Opens door -> hides -> closes door)
	UPROPERTY(EditAnywhere, BlueprintReadWrite/*, meta=(GetOptions="GetAvailableSlotIndices")*/)
	int32 MainSlotIndex = INDEX_NONE;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interaction|InteractionCascade")
	UInteractionCascadeSlot* MainSlot;

	//Current interaction index played
	UPROPERTY()
	int32 CurrentIndex = 0;

	UPROPERTY(EditAnywhere)
	EInteractionContext ExpectedContext;

	UPROPERTY()
	bool bIsComplete = true;
	TArray<FString> GetAvailableSlotIndices() const;

	//Refresh the main slot to see it in the editor
	void RefreshMainSlot();

	
};

#pragma endregion


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEHEIST_API UInteractableComponent : public UActorComponent, public IInteractionInterface
{
	GENERATED_BODY()

public:

	// PROPERTIES //

	
	//Event to notify the end of an interaction. Also will return the interacting actor and the interaction type
	UPROPERTY(BlueprintAssignable)
	FOnInteractionEndedEvent OnInteractionEndedEvent;

	//Array of every interaction created so far
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category="Interaction|Setup")
	TArray<UInteractionData*> AllInteractions; 

	// Array of all the components impacted by the interaction
	UPROPERTY(BlueprintReadOnly)
	TArray<USceneComponent*> AllComponentInteractable;
	
	// FUNCTIONS //

	// Sets default values for this component's properties
	UInteractableComponent();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	//Get all interactions for a told component
	UFUNCTION(BlueprintCallable)
	TArray<FString>GetInteractionsForAComp(USceneComponent* Comp);
	

	//Function to execute interaction (taking as an input the interaction text, received from the interaction player widget).
	//If no HitComponent provided, this will execute the interaction on a random component, implementing this interaction.
	UFUNCTION(BlueprintCallable, Category="Interaction|Setup")
	void InteractWithObject(const FString m_InteractText, USceneComponent* HitComponent, AActor* InteractingActor, EInteractionContext Context);

	//Function to execute interaction.
	//If no HitComponent provided, this will execute the interaction on a random component, implementing this interaction.
	//This one can be called, either with a specific interaction instance or with an interaction type. However, the second option will look for a random component implementing an interaction with this type
	//If a specific instance is provided, the interaction type WONT be used
	UFUNCTION(BlueprintCallable, Category="Interaction|Setup")
	void InteractWithSpecificInteraction(TSubclassOf<UInteractionData> InteractionType, USceneComponent* HitComponent, AActor* InteractingActor, EInteractionContext Context, UInteractionData* InteractionInstance);



protected:
	
	//------------Properties--------------//

	
	
	//Scene components which are accessing to this component
	UPROPERTY(Blueprintable,BlueprintReadWrite,  Category="Interactable")
	TArray<USceneComponent*>  AttachedComponents;

	//Actor owning the component
	UPROPERTY(VisibleAnywhere)
	AActor* Owner;
	
	//Determines if the object should implement the cascade interaction feature. If not, the component won't check any cascade.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	bool bShouldActivateCascade = false;
	
	
	
	//------------FUNCTIONS--------------//
	
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void OnRegister() override;
	
	//Wraps up the end of an interaction. Broadcast the dispatcher for the end of interacting
	UFUNCTION()
	void FinishInteraction(AActor* InteractingActor, UInteractionData* Interaction);

	
#if WITH_EDITOR
    
	//Triggered whenever a variable change is detected ONLY IN EDITOR MODE
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    	
#endif
	

	
#pragma region CascadeInteraction
	//-------------Cascade Interaction Mode--------------//

	//-------------Functions--------------//


	// Functions called by the inspector dynamic selection
	UFUNCTION()
	TArray<FName> GetAvailableInteractionComponents();

	// Functions called by the inspector dynamic selection. Gets an array of interactions names
	UFUNCTION()
	TArray<FName> GetAvailableInteractionsForSelectedComponent();

	//Returns all available cascade by their names
	UFUNCTION()
	TArray<FName> GetAvailableCascadeNames();

	//Execute the next interaction in the current cascade data
	void ExecuteNextCascadeInteraction(FInteractionCascadeData& Cascade, AActor* InteractingActor, EInteractionContext Context);

	//Returns the cascade available for these parameters. Need only interaction text or Interaction type to work.
	FInteractionCascadeData* FindValidCascade(const FString& m_InteractionText,EInteractionContext Context, const TSubclassOf<UInteractionData>& InteractionType,const UInteractionData* SpecificInteraction);
	
	//------------Properties--------------//

	//Array for all the cascades 
	UPROPERTY(EditAnywhere, Category="Interaction|InteractionCascade")
	TArray<FInteractionCascadeData> InteractionsCascadeDatas;

#pragma endregion
	
};
