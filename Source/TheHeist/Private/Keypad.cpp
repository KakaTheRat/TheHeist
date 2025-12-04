#include "Keypad.h"

APhysicalKeypad::APhysicalKeypad()
{
	PrimaryActorTick.bCanEverTick = false;

    // Création d'un mesh de base pour le boitier (optionnel)
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	EnigmaComponent = CreateDefaultSubobject<UEnigmaComponent>(TEXT("EnigmaComponent"));
    
    // Configuration de l'écran texte 3D
	ScreenText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ScreenText"));
	ScreenText->SetupAttachment(RootComponent);
    ScreenText->SetHorizontalAlignment(EHTA_Center);
    ScreenText->SetText(FText::FromString("----"));
    ScreenText->SetWorldSize(20.0f); // Taille du texte
}

void APhysicalKeypad::BeginPlay()
{
	Super::BeginPlay();
	CurrentCode = "";
    UpdateDisplay();
}

void APhysicalKeypad::ReceiveInput(FString Value)
{
    // Si on veut une touche "Clear" ou "Reset"
    if(Value == "C") 
    {
        CurrentCode = "";
        UpdateDisplay();
        return;
    }

    // Ajout du chiffre
	CurrentCode.Append(Value);
	UpdateDisplay();

    // Vérification via votre component existant
	if (EnigmaComponent && EnigmaComponent->NeedToCheckSolution(CurrentCode))
	{
		HandleValidation();
	}
}

void APhysicalKeypad::UpdateDisplay()
{
    if(ScreenText)
    {
        ScreenText->SetText(FText::FromString(CurrentCode));
    }
}

void APhysicalKeypad::HandleValidation()
{
	bool bSuccess = EnigmaComponent->TrySolveEnigma(CurrentCode);

	if (bSuccess)
	{
		ScreenText->SetTextRenderColor(FColor::Green);
		ScreenText->SetText(FText::FromString("OPEN"));
        // TODO : Ouvrir la porte ici ou appeler un Event
	}
	else
	{
		ScreenText->SetTextRenderColor(FColor::Red);
		ScreenText->SetText(FText::FromString("ERR"));
        
        // Timer pour reset après 1 seconde (Lambda function)
        FTimerHandle UnusedHandle;
        GetWorldTimerManager().SetTimer(UnusedHandle, [this]()
        {
            CurrentCode = "";
            ScreenText->SetTextRenderColor(FColor::White); // Remet en blanc
            UpdateDisplay();
        }, 1.5f, false);
	}
}