#include "KeypadButton.h"
#include "Keypad.h" // On inclut le header du keypad ici

AKeypadButton::AKeypadButton()
{
	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	RootComponent = ButtonMesh;
}

void AKeypadButton::InterractWithButton()
{
	if (LinkedKeypad)
	{
		// On appuie physiquement sur le bouton (petite animation optionnelle)
		// Puis on envoie la valeur au Keypad central
		LinkedKeypad->ReceiveInput(ButtonValue);
	}
}