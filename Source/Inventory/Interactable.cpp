// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory.h"
#include "Interactable.h"


// Sets default values
AInteractable::AInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	InteractableHelpText = FString("Press E to interact with Item");
}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

void AInteractable::Interact_Implementation()
{
	GLog->Log("Interact base class Interact() SHOULD NOT BE SEEING THIS");
}

