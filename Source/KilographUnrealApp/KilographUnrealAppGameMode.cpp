// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "KilographUnrealApp.h"
#include "KilographUnrealAppGameMode.h"
#include "KilographUnrealAppHUD.h"
#include "KilographUnrealAppCharacter.h"

AKilographUnrealAppGameMode::AKilographUnrealAppGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AKilographUnrealAppHUD::StaticClass();
}
