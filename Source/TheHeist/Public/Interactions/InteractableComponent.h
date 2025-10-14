// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionInterface.h"
#include "Components/ActorComponent.h"
#include "Player/PlayerInteractionComponent.h"
#include "../Structures/Interactions/InteractionData.h"
#include "Widget/Interaction/InteractionWidgetActor.h"
#include "InteractableComponent.generated.h"




//Structure used to match a scene component's name with an array of interactions linked to this one.
USTRUCT(BlueprintType)
struct FInteractionEntry
{
	GENERATED_BODY()

	// Associated scene component name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction|Setup")
	FName ComponentName;

	// Array of available interactions types for this component
	UPROPERTY(EditAnywhere,Instanced, BlueprintReadWrite, Category="Interaction|Setup")
	TArray<UInteractionData*> Interactions; 
};

//Structure used to match a scene component's name to a single interaction. Used for the interaction cascade  
USTRUCT(BlueprintType)
struct FInteractionCascadeSlot
{
	GENERATED_BODY()

	// Associated scene component name
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ComponentName;

	//Associated interaction
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInteractionData* InteractionData;
	
};

//Structure used for the interaction cascade management. Each cascade has an array of single interaction
USTRUCT(BlueprintType)
struct FInteractionCascadeData
{
	GENERATED_BODY()
	
	//Name to recognize a particular cascade struct
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CascadeName;

	//Array of single interactions matched with a component name
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInteractionCascadeSlot> InteractionCascades;

	// === Index du slot principal (dropdown auto-généré) ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(GetOptions="GetAvailableSlotIndices"))
	int32 MainSlotIndex = INDEX_NONE;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interaction|InteractionCascade")
	FInteractionCascadeSlot MainSlot;

	// ------- Functions
	TArray<FString> GetAvailableSlotIndices() const;
	void RefreshMainSlot();
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEHEIST_API UInteractableComponent : public UActorComponent, public IInteractionInterface
{
	GENERATED_BODY()

public:

	//-----------Functions--------------//
	
	//----------Properties--------------//



	
	//-------------Cascade Interaction Mode--------------//

	//-------------Functions--------------//


	// Functions called by the inspector dynamic selection
	UFUNCTION()
	TArray<FName> GetAvailableInteractionComponents();

	// Functions called by the inspector dynamic selection. Gets an array of interactions names
	UFUNCTION()
	TArray<FName> GetAvailableInteractionsForSelectedComponent();

	//Add chosen interaction to the currently chosen cascade 
	UFUNCTION(CallInEditor, Category="Interaction|InteractionCascade", meta=(EditCondition="bIsCascadeActive"))
	void AddCascadeInteraction();

	UFUNCTION()
	TArray<FName> GetAvailableCascadeNames();

	//Execute the next interaction in the current cascade data
	void ExecuteNextCascadeInteraction();
	
	//------------Properties--------------//
	
	UPROPERTY(EditAnywhere, Category="Interaction|InteractionCascade", meta=(EditCondition = "bShouldActivateCascade"))
	TArray<FInteractionCascadeData> InteractionsCascadeDatas;
	
	// Display in the editor the component linked with data
	UPROPERTY(EditAnywhere, Category="Interaction|InteractionCascade", meta=(GetOptions="GetAvailableInteractionComponents",EditCondition="bShouldActivateCascade"))
	FName SelectedComponentName;

	// Display in the editor the datas linked to the chosen component
	UPROPERTY(EditAnywhere, Category="Interaction|InteractionCascade", meta=(GetOptions="GetAvailableInteractionsForSelectedComponent", EditCondition="bShouldActivateCascade"))
	FName SelectedInteractionName;

	UPROPERTY(EditAnywhere, Category="Interaction|InteractionCascade", meta=(GetOptions="GetAvailableCascadeNames", EditCondition="bShouldActivateCascade"))
	FName SelectedCascadeName;
	
	UPROPERTY(EditAnywhere, Category="Interaction|InteractionCascade", meta=(EditCondition="bShouldActivateCascade"))
	FName NewCascadeName;

	//Current cascade being interacted with at the moment
	FInteractionCascadeData* CurrentCascade = nullptr;

	//Current cascade index
	int32 CurrentCascadeIndex = 0;


	
	

#if WITH_EDITOR
    
	//Triggered whenever a variable change is detected ONLY IN EDITOR MODE
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    	
#endif
	
	


protected:

	//-------------Functions--------------//

	
	
	//------------Properties--------------//
	
	//Scene components which are accessing to this component
	UPROPERTY(Blueprintable,BlueprintReadWrite,  Category="Interactable")
	TArray<USceneComponent*>  AttachedComponents;

	//Actor owning the component
	UPROPERTY()
	AActor* Owner;

	//Component targeted by the player
	UPROPERTY(BlueprintReadOnly)
	USceneComponent* CurrentlyChosenComponent;

	//Current entry treated
	FInteractionEntry* CurrentEntry = nullptr;

	//Array of interactions config created so far
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	TArray<FInteractionEntry> InteractionsConfig;

	//Array of interactions config created so far with the perception activated
	UPROPERTY(BlueprintReadWrite)
	TArray<FInteractionEntry> InteractionsConfigPerceptionAI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	bool bShouldActivateCascade = false;

	//----------WIDGET--------------//
	
	// Widget Class to instantiate. Widget for the interaction system
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<AInteractionWidgetActor> InteractionWidgetClass;

	//Widget instance
	AInteractionWidgetActor* WidgetInstance = nullptr;
	
	

	//------------FUNCTIONS--------------//
	
	// Called when the game starts
	virtual void BeginPlay() override;
	
	//Function to execute interaction (taking as an input the interaction text, received from the widget
	UFUNCTION(BlueprintCallable, Category="Interaction|Setup")
	void InteractWithObject(const FString m_InteractText);

	//Implementation for the interact interface function
	virtual void Interact_Implementation(USceneComponent* HitComponent, AActor* InteractingActor) override;

	//Implementation for the interact AI interface function
	virtual void InteractAI_Implementation() override;



	

public:
	
	// Sets default values for this component's properties
	UInteractableComponent();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
};
