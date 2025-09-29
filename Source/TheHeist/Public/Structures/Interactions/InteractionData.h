#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Enumerators/Interactions/InteractionsEnum.h"
#include "InteractionData.generated.h"

UCLASS(Abstract, Blueprintable, EditInlineNew)
class THEHEIST_API UInteractionData : public UObject
{
    GENERATED_BODY()

public:

    // Displayed text on the interaction widget
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
    FString InteractText;
    
    //Scene component name attached to this interaction
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
    FString AttachedComponentName;
};

UCLASS(Blueprintable, EditInlineNew)
class THEHEIST_API UOpenableData : public UInteractionData
{
    GENERATED_BODY()

public:

    //Allow to choose the type of opening wanted
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Openable")
	EOpenableType openableType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Openable")
    bool bIsOpened = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Openable")
    float OpenProgress = 0.f;
    
    // Curve to control the animation
    UPROPERTY(EditAnywhere, Category="Openable")
    UCurveFloat* Curve;
    
    // Optional length of animation (curve can have its own length)
    UPROPERTY(EditAnywhere, Category="Openable")
    float Duration = 1.0f;
};

UCLASS(Blueprintable, EditInlineNew)
class THEHEIST_API UActivableData : public UInteractionData
{
    GENERATED_BODY()

public:

    //Light intensity
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Activable")
    float LightIntensity = 1.f;
    
    //Activation state
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Activable")
    bool bIsActivated;
    
    //Allow to choose the type of activation wanted
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Activable")
    EActivableType activationType;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Activable")
    USoundBase* Sound;
};

UCLASS(Blueprintable, EditInlineNew)
class THEHEIST_API UHideableData : public UInteractionData
{
    GENERATED_BODY()

public:

    //Hidding state
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hideable")
    bool bIsUsed;

    //String for the arrow component used for the hidding position
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hideable")
    FString HiddenArrowName;

    //String for the arrow component used for the quitting position
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hideable")
    FString QuitArrowName;
    
};

UCLASS(Blueprintable, EditInlineNew)
class THEHEIST_API UCollectableData : public UInteractionData
{
    GENERATED_BODY()

public:

    //Collectable quantity 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectable")
    int32 Quantity = 1;
    
};