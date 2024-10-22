// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Inventory.h"
#include "InventoryCharacter.h"
#include "InventoryProjectile.h"
#include "InventoryGameMode.h"
#include "Animation/AnimInstance.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "GameFramework/InputSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AInventoryCharacter

AInventoryCharacter::AInventoryCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	Reach = 250.f;
	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void AInventoryCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint")); //Attach gun mesh component to Skeleton, doing it here because the skelton is not yet created in the constructor

	Inventory.SetNum(4);

	CurrentInteractable = nullptr;
}

void AInventoryCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckForInteractables();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AInventoryCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);


	InputComponent->BindAction("Crouch", IE_Pressed, this, &AInventoryCharacter::StartCrouch);
	InputComponent->BindAction("Crouch", IE_Released, this, &AInventoryCharacter::EndCrouch);


	InputComponent->BindAction("Interact", IE_Pressed, this, &AInventoryCharacter::Interact);
	InputComponent->BindAction("ToggleInventory", IE_Pressed, this, &AInventoryCharacter::ToggleInventory);

	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AInventoryCharacter::TouchStarted);
	if (EnableTouchscreenMovement(InputComponent) == false)
	{
		InputComponent->BindAction("Fire", IE_Pressed, this, &AInventoryCharacter::OnFire);
	}

	InputComponent->BindAxis("MoveForward", this, &AInventoryCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AInventoryCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AInventoryCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AInventoryCharacter::LookUpAtRate);
}

void AInventoryCharacter::UpdateGold(int32 Amount)
{
	Gold += Amount;
}

bool AInventoryCharacter::AddItemToInventory(APickup * Item)
{
	if (Item)
	{
		const int32 AvailableSlot = Inventory.Find(nullptr); // Find the first slot that is empty.
		if (AvailableSlot != INDEX_NONE)
		{
			Inventory[AvailableSlot] = Item;
			return true;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("You can not carry any more items!"));
			return false;
		}
	}

	return false;
}

UTexture2D * AInventoryCharacter::GetThumbnailAtIntentorySlot(int32 Slot)
{
	if (Inventory[Slot])
	{
		return Inventory[Slot]->PickupThumbnail;
	}
	return nullptr;
}

FString AInventoryCharacter::GetItemNameAtInventorySlot(int32 Slot)
{
	if (Inventory[Slot])
	{
		return Inventory[Slot]->ItemName;
	}
	return FString("None");
}

void AInventoryCharacter::UseItemAtInventorySlot(int32 Slot)
{
	if (Inventory[Slot])
	{
		Inventory[Slot]->Use_Implementation();
		Inventory[Slot] = NULL; // deletes the item from inventory after used.
	}
}

bool AInventoryCharacter::StartInspecting(AInspectable* Item)
{
	if (Item)
	{
		Item->TeleportTo(this->GetPawnViewLocation(), this->GetActorRotation());
		
		this->Controller->SetIgnoreMoveInput(false);
		this->SetActorEnableCollision(false);

		//FirstPersonCameraComponent->Activate(false);
		//FirstPersonCameraComponent->UserControll
		//FirstPersonCameraComponent->bUsePawnControlRotation = false;
		//this->bUseControllerRotationYaw = false;
		return true;
	}

	return false;
}

void AInventoryCharacter::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		const FRotator SpawnRotation = GetControlRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			// spawn the projectile at the muzzle
			World->SpawnActor<AInventoryProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}

}

void AInventoryCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AInventoryCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = false;
}

void AInventoryCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
	{
		if (TouchItem.bIsPressed)
		{
			if (GetWorld() != nullptr)
			{
				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
				if (ViewportClient != nullptr)
				{
					FVector MoveDelta = Location - TouchItem.Location;
					FVector2D ScreenSize;
					ViewportClient->GetViewportSize(ScreenSize);
					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.X * BaseTurnRate;
						AddControllerYawInput(Value);
					}
					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.Y * BaseTurnRate;
						AddControllerPitchInput(Value);
					}
					TouchItem.Location = Location;
				}
				TouchItem.Location = Location;
			}
		}
	}
}

void AInventoryCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AInventoryCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AInventoryCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AInventoryCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AInventoryCharacter::EnableTouchscreenMovement(class UInputComponent* InputComponent)
{
	bool bResult = false;
	if (FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		bResult = true;
		InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AInventoryCharacter::BeginTouch);
		InputComponent->BindTouch(EInputEvent::IE_Released, this, &AInventoryCharacter::EndTouch);
		InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AInventoryCharacter::TouchUpdate);
	}
	return bResult;
}

void AInventoryCharacter::StartCrouch()
{
	//if (GetCharacterMovement()->IsMovingOnGround())
	//{
	//	Crouch();
	//}

	if (GetCharacterMovement()->CanCrouchInCurrentState())
	{
		GetCharacterMovement()->bWantsToCrouch = true;
		//Crouch(true);
	}
}

void AInventoryCharacter::EndCrouch()
{
	// if (GetCharacterMovement()->IsMovingOnGround())
    // {
	//	UnCrouch();
    // }

	if (GetCharacterMovement()->CanCrouchInCurrentState())
	{
		GetCharacterMovement()->bWantsToCrouch = false;
		//UnCrouch(true);
	}
}

void AInventoryCharacter::ToggleInventory()
{
	/*Check players hud state, if inventory is open then close it, otherwise open inventory.*/
	AInventoryGameMode* GameMode = Cast<AInventoryGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode->GetHUDState() == GameMode->HS_Ingame)
	{
		GameMode->ChangeHudState(GameMode->HS_Inventory);
	}
	else
	{
		GameMode->ChangeHudState(GameMode->HS_Ingame);
	}
}

void AInventoryCharacter::Interact()
{
	if (CurrentInteractable)
	{
		CurrentInteractable->Interact_Implementation();
	}
}

void AInventoryCharacter::CheckForInteractables()
{
	// To linetrace, get the start and traces
	FVector StartTrace = FirstPersonCameraComponent->GetComponentLocation();
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	FVector EndTrace = (ForwardVector * Reach) + StartTrace;

	// Declare a hitresult to store the raycast hit in

	FHitResult* HitResult = new FHitResult();

	// Initialize the query params - ignore the actor

	FCollisionQueryParams CQP;
	CQP.AddIgnoredActor(this);

	//Cast the line trace         ECC_Visibility   ECC_WorldDynamic
	//GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, EndTrace, ECC_WorldDynamic, CQP);

	if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, EndTrace, ECC_Visibility, CQP))
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, false, 5.f);

	AInteractable* PotentialInteractable = Cast<AInteractable>(HitResult->GetActor());

	if (PotentialInteractable == NULL)
	{
		HelpText = FString("");
		CurrentInteractable = nullptr;
		return;
	}
	else
	{
		CurrentInteractable = PotentialInteractable;
		HelpText = PotentialInteractable->InteractableHelpText;
	}
}
