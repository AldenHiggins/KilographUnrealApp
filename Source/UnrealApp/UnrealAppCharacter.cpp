// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UnrealApp.h"
#include "UnrealAppCharacter.h"
#include "UnrealAppProjectile.h"
#include "CameraFollow.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
///////////////////////  CONSTRUCTOR  ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
AUnrealAppCharacter::AUnrealAppCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 90.f;
	BaseLookUpRate = 90.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	// Initialize state to freerun
	state = FREERUN;

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
///////////////////////  INITIALIZATION  /////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void AUnrealAppCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Initialize camera follow
	if (cameraFollowActor != NULL)
	{
		cameraFollow = cameraFollowActor->FindComponentByClass<UCameraFollow>();
		cameraFollow->setPlayer(this);
		
		//UE_LOG(Kilograph, Log, TEXT("Initialized player position to orbit around: %s"), cameraFollow);
	}

	// Start the player at the correct orbiting position
	if (rotationObject != NULL)
	{
		UE_LOG(Kilograph, Log, TEXT("Initialized player position to orbit around: %s"), *rotationObject->GetName());
		state = ORBIT;
		currentXRotationAroundObject = 0;
		currentZRotationAroundObject = 0;
		orbitReposition();
	}

	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AUnrealAppCharacter::TouchStarted);
	if( EnableTouchscreenMovement(InputComponent) == false )
	{
		InputComponent->BindAction("Fire", IE_Pressed, this, &AUnrealAppCharacter::OnFire);
	}
	
	InputComponent->BindAxis("MoveForward", this, &AUnrealAppCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AUnrealAppCharacter::MoveRight);
	
	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AUnrealAppCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AUnrealAppCharacter::LookUpAtRate);
}

//////////////////////////////////////////////////////////////////////////
///////////////////////  TOUCH FUNCTIONS  ////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void AUnrealAppCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
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

void AUnrealAppCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
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

void AUnrealAppCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
	{
		if (!TouchItem.bIsPressed)
		{
			return;
		}

		if (GetWorld() == nullptr)
		{
			return;
		}

		UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
		if (ViewportClient != nullptr)
		{
			FVector MoveDelta = Location - TouchItem.Location;
			FVector2D ScreenSize;
			ViewportClient->GetViewportSize(ScreenSize);
			FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
			if (ScaledDelta.X != 0.0f)
			{
				TouchItem.bMoved = true;
				float Value = ScaledDelta.X * BaseTurnRate;
				tapDragX(Value);
			}
			if (ScaledDelta.Y != 0.0f)
			{
				TouchItem.bMoved = true;
				float Value = ScaledDelta.Y* BaseTurnRate;
				tapDragY(Value);
			}
			TouchItem.Location = Location;
		}
		TouchItem.Location = Location;
	}
}

//////////////////////////////////////////////////////////////////////////
////////////////////  STATE TOUCH FUNCTIONS  /////////////////////////////
//////////////////////////////////////////////////////////////////////////
void AUnrealAppCharacter::tapDragX(float Value)
{
	switch (state)
	{
		case FREERUN:
		{
			AddControllerYawInput(Value);
			break;
		}
		case ORBIT:
		{
			UE_LOG(Kilograph, Log, TEXT("Delta X: %f"), Value);
			currentZRotationAroundObject += Value;
			orbitReposition();
			break;
		}
	}
}

void AUnrealAppCharacter::tapDragY(float Value)
{
	switch (state)
	{
		case FREERUN:
		{
			AddControllerPitchInput(Value);
			break;
		}
		case ORBIT:
		{
			UE_LOG(Kilograph, Log, TEXT("Delta Y: %f"), Value);
			currentXRotationAroundObject -= Value;
			orbitReposition();
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
/////////////////////////      ORBIT       ///////////////////////////////
//////////////////////////////////////////////////////////////////////////
void AUnrealAppCharacter::orbitReposition()
{
	UE_LOG(Kilograph, Log, TEXT("Orbiting"));
	FRotator rotation = FRotator::MakeFromEuler(FVector(currentXRotationAroundObject, 0.0f, currentZRotationAroundObject));
	//Rotation Matrix
	FRotationMatrix MyRotationMatrix(rotation);

	FVector rotatedPosition = MyRotationMatrix.TransformVector(FVector(0.0f, rotationDistance, 0.0f));
	SetActorLocation(rotationObject->GetActorLocation() + rotatedPosition);
	this->GetController()->SetControlRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), rotationObject->GetActorLocation()));
}

//////////////////////////////////////////////////////////////////////////
///////////////////////  BUTTON CALLBACKS  ///////////////////////////////
//////////////////////////////////////////////////////////////////////////
void AUnrealAppCharacter::activateCameraFollow()
{
	tapDragY(1);
	state = FREERUN;
	UE_LOG(Kilograph, Log, TEXT("STARTED CAMERA FOLLOWING WHOOOO"));
	cameraFollow->startFollowing();
}

//////////////////////////////////////////////////////////////////////////
///////////////////////  OTHER/MISC/LEGACY  //////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool AUnrealAppCharacter::EnableTouchscreenMovement(class UInputComponent* InputComponent)
{
	bool bResult = false;
	if (FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		bResult = true;
		InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AUnrealAppCharacter::BeginTouch);
		InputComponent->BindTouch(EInputEvent::IE_Released, this, &AUnrealAppCharacter::EndTouch);
		InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AUnrealAppCharacter::TouchUpdate);
	}
	return bResult;
}

void AUnrealAppCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AUnrealAppCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AUnrealAppCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AUnrealAppCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AUnrealAppCharacter::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		const FRotator SpawnRotation = GetControlRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(GunOffset);

		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			// spawn the projectile at the muzzle
			World->SpawnActor<AUnrealAppProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
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