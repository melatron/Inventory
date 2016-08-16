// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory.h"
#include "Pickup.h"
#include "InventoryCharacter.h"


APickup::APickup()
{
	//set up the mesh for the pickup and set up item name, help text and item value.
	InteractableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	InteractableMesh->SetSimulatePhysics(true);

	ItemName = FString("Enter the item name here...");
	InteractableHelpText = FString("Pres E to pick the item up.");
	Value = 0;
}

void APickup::BeginPlay()
{
	InteractableHelpText = FString::Printf(TEXT("%s: Press e to pick up."), *ItemName);
}

void APickup::Interact_Implementation()
{
	AInventoryCharacter* Character = Cast<AInventoryCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	//Put code here that places the item into the characters inventory
}

void APickup::Use_Implementation()
{
	GLog->Log("Use() called from pickup base class SHOULD NOT SEE THIS");
}

void APickup::onPickedUp()
{
	InteractableMesh->SetVisibility(false);
	InteractableMesh->SetSimulatePhysics(false);
	InteractableMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

