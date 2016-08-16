// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "InventoryHUD.generated.h"

UCLASS()
class AInventoryHUD : public AHUD
{
	GENERATED_BODY()

public:
	AInventoryHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

