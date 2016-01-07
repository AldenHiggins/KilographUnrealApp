// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "KilographUnrealAppHUD.generated.h"

UCLASS()
class AKilographUnrealAppHUD : public AHUD
{
	GENERATED_BODY()

public:
	AKilographUnrealAppHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

