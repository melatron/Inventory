// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory.h"
#include "InventoryCharacter.h"
#include "Pickup_Coins.h"

APickup_Coins::APickup_Coins()
{
	AmountOfCoins = 0;
}

void APickup_Coins::Interact_Implementation()
{
	AInventoryCharacter* Character = Cast<AInventoryCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	Character->UpdateGold(AmountOfCoins);

	Destroy();
}
