// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "InventoryGameMode.generated.h"

UCLASS(minimalapi)
class AInventoryGameMode : public AGameMode
{
	GENERATED_BODY()

		virtual void BeginPlay() override;
public:
	AInventoryGameMode();

	enum EHUDState : uint8
	{
		HS_Ingame,
		HS_Inventory,
		HS_Shop_General,
		HS_Shop_Weapon
	};	

	/*Checks the HUD state and then calls applyHUD accordingly to apply whatever hud we want.*/

	void ApplyHUDChanges();

	uint8 GetHUDState();

	UFUNCTION(BlueprintCallable, Category = "HUD Functions")
	void ChangeHudState(uint8 NewState);

	/*Applies HUD to the screen and returns true if successful.*/
	bool ApplyHUD(TSubclassOf<class UUserWidget> WidgetToApply, bool bShowMouseCursor, bool EnableClickEvents);

protected:
	uint8 HUDState;
	/*The HUD to be shown ingame*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HUDWidgets", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> IngameHUDClass;

	/*The HUD to be shown in the inventory*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HUDWidgets", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> InventoryHUDClass;

	/*The HUD to be shown in the shop*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HUDWidgets", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> ShopGeneralHUDClass;

	UPROPERTY()
	class UUserWidget* CurrentWidget;
};



