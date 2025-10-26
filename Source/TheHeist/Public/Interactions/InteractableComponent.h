// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionInterface.h"
#include "Components/ActorComponent.h"
#include "Player/PlayerInteractionComponent.h"
#include "../Structures/Interactions/InteractionData.h"
#include "Widget/Interaction/InteractionWidgetActor.h"
#include "InteractableComponent.generated.h"



#pragma region InteractionStruct

//Structure used to match a scene component's name with an array of interactions linked to this one.
USTRUCT(BlueprintType)
struct FInteractionEntry
{
	GENERATED_BODY()

	// Associated scene component name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction|Setup")
	FName ComponentName;

	// Array of available interactions types for this component
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category="Interaction|Setup")
	TArray<UInteractionData*> Interactions; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction|Setup")
	TArray<TSoftObjectPtr<UInteractionData>> InteractionRefs;
	
	void UpdateSoftReferences()
	{
		InteractionRefs.Empty();

		for (UInteractionData* Data : Interactions)
		{
			if (IsValid(Data))
			{
				InteractionRefs.Add(Data);
			}
		}
	}

	
};
#pragma endregion 

#pragma region CascadeSlotStruct
/*
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
	TSoftObjectPtr<UInteractionData> InteractionData;
	
	// Expected state before executing this interaction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(GetOptions="GetAvailableStates"))
	FName ExpectedState;

	// Array of available states, feeding the exepected state dropdown
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FName> AvailableStates;

	TArray<FName> GetAvailableStates() const { return AvailableStates; }
};*/
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class UInteractionCascadeSlot : public UObject
{
	GENERATED_BODY()

public:

	// Display in the editor the component linked with data
	UPROPERTY(EditAnywhere,  meta=(GetOptions="GetAvailableInteractionComponents"))
	FName SelectedComponentName;
	
	// Functions called by the inspector dynamic selection
	UFUNCTION()
	TArray<FName> GetAvailableInteractionComponents();

	// Functions called by the inspector dynamic selection. Gets an array of interactions names
	UFUNCTION()
	TArray<FName> GetAvailableInteractionsForSelectedComponent();

	// Display in the editor the datas linked to the chosen component
	UPROPERTY(EditAnywhere, meta=(GetOptions="GetAvailableInteractionsForSelectedComponent"))
	FName SelectedInteractionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UInteractionData> InteractionData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(GetOptions="GetAvailableStates"))
	FName ExpectedState;

	UFUNCTION()
	TArray<FName> GetAvailableStates() const;

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

	UPROPERTY()
	int32 CurrentIndex = 0;
	
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

	
#if WITH_EDITOR
    
	//Triggered whenever a variable change is detected ONLY IN EDITOR MODE
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    	
#endif
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USceneComponent*> AllComponentInteractable;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category="Interaction|Setup")
	TArray<UInteractionData*> AllInteractions; 


protected:
	
	//------------Properties--------------//
	
	//Scene components which are accessing to this component
	UPROPERTY(Blueprintable,BlueprintReadWrite,  Category="Interactable")
	TArray<USceneComponent*>  AttachedComponents;

	//Actor owning the component
	UPROPERTY(VisibleAnywhere)
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

	//Determines if the object should implement the cascade interaction feature. If not, the component won't check any cascade.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	bool bShouldActivateCascade = false;
	

	//------------FUNCTIONS--------------//
	
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void OnRegister() override;
	
	
	//Function to execute interaction (taking as an input the interaction text, received from the widget
	UFUNCTION(BlueprintCallable, Category="Interaction|Setup")
	void InteractWithObject(const FString m_InteractText);

	//Implementation for the interact interface function
	virtual void Interact_Implementation(USceneComponent* HitComponent, AActor* InteractingActor, EInteractionContext Context) override;

	//Implementation for the interact AI interface function
	virtual void InteractAI_Implementation() override;



	

public:
	
	// Sets default values for this component's properties
	UInteractableComponent();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;




	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool b_hiden;



	

private:

	UPROPERTY()
	AActor* InteractingActorr = nullptr;
	
	EInteractionContext CurrentInteractionContext = EInteractionContext::Default;
	
#pragma region CascadeInteraction
	//-------------Cascade Interaction Mode--------------//

	//-------------Functions--------------//


	// Functions called by the inspector dynamic selection
	UFUNCTION()
	TArray<FName> GetAvailableInteractionComponents();

	// Functions called by the inspector dynamic selection. Gets an array of interactions names
	UFUNCTION()
	TArray<FName> GetAvailableInteractionsForSelectedComponent();

	//Add chosen interaction to the currently chosen cascade 
	UFUNCTION(CallInEditor, Category="Interaction|InteractionCascade", meta =(EditCondition="bShouldActivateCascade"))
	void AddCascadeInteraction();

	//Returns all available cascade by their names
	UFUNCTION()
	TArray<FName> GetAvailableCascadeNames();

	//Execute the next interaction in the current cascade data
	void ExecuteNextCascadeInteraction(FInteractionCascadeData& Cascade);
	
	
	//------------Properties--------------//
	
	UPROPERTY(EditAnywhere, Category="Interaction|InteractionCascade")
	TArray<FInteractionCascadeData> InteractionsCascadeDatas;
	
	// Display in the editor the component linked with data
/*	UPROPERTY(EditAnywhere, Category="Interaction|InteractionCascade", meta=(GetOptions="GetAvailableInteractionComponents",EditCondition="bShouldActivateCascade"))
	FName SelectedComponentName;

	// Display in the editor the datas linked to the chosen component
	UPROPERTY(EditAnywhere, Category="Interaction|InteractionCascade", meta=(GetOptions="GetAvailableInteractionsForSelectedComponent", EditCondition="bShouldActivateCascade"))
	FName SelectedInteractionName;

	UPROPERTY(EditAnywhere, Category="Interaction|InteractionCascade", meta=(GetOptions="GetAvailableCascadeNames", EditCondition="bShouldActivateCascade"))
	FName SelectedCascadeName;*/
	
	UPROPERTY(EditAnywhere, Category="Interaction|InteractionCascade", meta=(EditCondition="bShouldActivateCascade"))
	FName NewCascadeName;


#pragma endregion
	
};
