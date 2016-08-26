// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup.h"
#include "Pickup_Coins.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORY_API APickup_Coins : public APickup
{
	GENERATED_BODY()

public:
	APickup_Coins();

	virtual void Interact_Implementation() override;
	
	UPROPERTY(EditAnywhere, Category = "Coin Properties.")
	int32 AmountOfCoins;
};
