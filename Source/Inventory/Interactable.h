// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

UCLASS()
class INVENTORY_API AInteractable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractable();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame

	UFUNCTION(BlueprintNativeEvent)
	void Interact();
	virtual void Interact_Implementation();
	
	/*We can assume all interactables will have a static mesh*/
	UPROPERTY(EditAnywhere, Category = "Interatable Property")
	UStaticMeshComponent* InteractableMesh;

	/*We can assume all interactables will have a help text*/
	UPROPERTY(EditAnywhere, Category = "Interatable Property")
	FString InteractableHelpText;

};
