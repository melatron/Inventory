// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "Interactable.h"
#include "Pickup.h"
#include "Public/Inspectable.h"
#include "InventoryCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class AInventoryCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;
public:
	AInventoryCharacter();

	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AInventoryProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	/*The players help text*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	FString HelpText;

	/*The amount of gold the player has*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	int32 Gold;

	/*
	Updates the gold
	@params Amount this is the amount to update the coins by -> it can be positive or negative.
	*/
	UFUNCTION(BlueprintCallable, Category = "Inventory Functions")
	void UpdateGold(int32 Amount);

	/*Adds an item to the inventory*/
	UFUNCTION(BlueprintPure, Category = "Inventory Functions")
	bool AddItemToInventory(APickup* Item);

	/*Gets the thumbnail for a given inventory slot*/
	UFUNCTION(BlueprintPure, Category = "Inventory Functions")
	UTexture2D* GetThumbnailAtIntentorySlot(int32 Slot);

	/*Gets the item name for a given inventory slot*/
	UFUNCTION(BlueprintPure, Category = "Inventory Functions")
	FString GetItemNameAtInventorySlot(int32 Slot);

	/*Uses the item at a given inventory slot*/
	UFUNCTION(BlueprintCallable, Category = "Inventory Functions")
	void UseItemAtInventorySlot(int32 Slot);

	/*The players inventory represented as TArray of pickup objects*/
	UPROPERTY(EditAnywhere)
	TArray<APickup*> Inventory;


	// Inspecting
	UFUNCTION(BlueprintPure, Category = "Inspecting Functions")
		bool StartInspecting(AInspectable* Item);

protected:
	
	/** Fires a projectile. */
	void OnFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

private:
	/*The players reach*/
	float Reach;

	/*Crouch*/
	void StartCrouch();
	void EndCrouch();

	/*Toggles the inventory*/
	void ToggleInventory();
	/*Interact with the current interactable if there is one*/
	void Interact();

	/*Checks for interactable items directly in front of the player using a linetrace, called on a per tich basic*/
	void CheckForInteractables();

	/*The interactable the player is curently looking at*/
	AInteractable* CurrentInteractable;

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

