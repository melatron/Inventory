// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup.h"
#include "Pickup_Chair.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORY_API APickup_Chair : public APickup
{
	GENERATED_BODY()

public:
		APickup_Chair();

		virtual void Use_Implementation() override;
	
};
