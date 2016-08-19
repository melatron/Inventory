// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Inventory.h"
#include "InventoryGameMode.h"
#include "InventoryHUD.h"
#include "InventoryCharacter.h"
#include "Blueprint/UserWidget.h"

void AInventoryGameMode::BeginPlay()
{
	//As soon the player enters the game, apply hud to the screen
	ApplyHUDChanges();
}

AInventoryGameMode::AInventoryGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AInventoryHUD::StaticClass();

	// set the default state to be in game hud.
	HUDState = EHUDState::HS_Ingame;
}

void AInventoryGameMode::ApplyHUDChanges()
{
	// Remove the previous hud, since we are applying new one
	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromParent();
	}

	switch (HUDState)
	{
		case EHUDState::HS_Ingame: {
			ApplyHUD(IngameHUDClass, false, false);
			break;
		}
		case EHUDState::HS_Inventory: {
			ApplyHUD(InventoryHUDClass, true, true);
			break;
		}
		case EHUDState::HS_Shop_General: {
			ApplyHUD(ShopGeneralHUDClass, true, true);
			break;
		}
		default: {
			ApplyHUD(IngameHUDClass, false, false);
			break;
		}
	}
}

uint8 AInventoryGameMode::GetHUDState()
{
	return HUDState;
}

void AInventoryGameMode::ChangeHudState(uint8 NewState)
{
	HUDState = NewState;
	ApplyHUDChanges();
}

bool AInventoryGameMode::ApplyHUD(TSubclassOf<class UUserWidget> WidgetToApply, bool bShowMouseCursor, bool EnableClickEvents)
{
	// Get a reference to the player and the controller.
	AInventoryCharacter* MyCharacter = Cast<AInventoryCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	APlayerController* MyController = GetWorld()->GetFirstPlayerController();

	// Nullcheck whatever widget we're trying to apply.
	if (WidgetToApply)
	{
		//Set mouse visibility and click events according to params
		MyController->bShowMouseCursor = bShowMouseCursor;
		MyController->bEnableClickEvents = EnableClickEvents;

		//Create widget
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetToApply);

		if (CurrentWidget)	
		{
			CurrentWidget->AddToViewport();
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}
