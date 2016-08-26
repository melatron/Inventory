// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory.h"
#include "Inspectable.h"
#include "InventoryCharacter.h"
#include "../Public/Inspectable.h"


AInspectable::AInspectable()
{
	//set up the mesh for the pickup and set up item name, help text and item value.
	InteractableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	InteractableMesh->SetSimulatePhysics(true);

	ItemName = FString("Enter the item name here...");
	InteractableHelpText = FString("Pres E to inspect the item.");
}

void AInspectable::BeginPlay()
{
}

void AInspectable::Interact_Implementation()
{
	AInventoryCharacter* Character = Cast<AInventoryCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	CurrentLocation = this->GetActorLocation();

	if (Character->StartInspecting(this))
	{
		onInspected();
	}
}

void AInspectable::onInspected()
{
	InteractableMesh->SetVisibility(true);
	InteractableMesh->SetSimulatePhysics(false);
	InteractableMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AInspectable::onStopInspected()
{
	InteractableMesh->SetVisibility(true);
	InteractableMesh->SetSimulatePhysics(true);
	InteractableMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}
