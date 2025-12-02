#include "PuzzleKeyPadButton.h"

#include "PuzzleKeyPad.h"

APuzzleKeyPadButton::APuzzleKeyPadButton()
{
	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	RootComponent = ButtonMesh;
}

void APuzzleKeyPadButton::InterractWithButton()
{
	if (LinkedKeypad)
	{
		// On appuie physiquement sur le bouton (petite animation optionnelle)
		// Puis on envoie la valeur au Keypad central
		LinkedKeypad->ReceiveInput(ButtonValue);
	}
}
