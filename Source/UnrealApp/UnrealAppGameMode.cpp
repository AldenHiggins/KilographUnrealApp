// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UnrealApp.h"
#include "UnrealAppGameMode.h"
#include "UnrealAppHUD.h"
#include "UnrealAppCharacter.h"

AUnrealAppGameMode::AUnrealAppGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AUnrealAppHUD::StaticClass();
}
