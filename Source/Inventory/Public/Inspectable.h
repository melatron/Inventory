// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Interactable.h"
#include "Inspectable.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORY_API AInspectable : public AInteractable
{
	GENERATED_BODY()
	
	
//public:
	AInspectable();

	virtual void BeginPlay() override;
	virtual void Interact_Implementation() override;

	UPROPERTY(EditAnywhere, Category = "Pickup Properties")
		FString ItemName;

	FVector CurrentLocation;

	/*This is called when the item is inspected and need to move to the camera*/
	void onInspected();
	void onStopInspected();

	
};
