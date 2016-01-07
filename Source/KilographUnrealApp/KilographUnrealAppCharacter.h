// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "CameraFollow.h"
#include "GameFramework/Character.h"
#include "KilographUnrealAppCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class AKilographUnrealAppCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;
public:
	AKilographUnrealAppCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector GunOffset;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	// Object around which the user will rotate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Control)
	class AActor* rotationObject;

	// Distance at which the player will rotate around the rotation object
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Control)
	float rotationDistance;

	// Distance at which the player will rotate around the rotation object
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Control)
	class AActor* cameraFollowActor;

	// Function callback to activate camera following
	UFUNCTION(BlueprintCallable, Category = "Custom")
	void activateCameraFollow();

	// Function callback to activate camera following
	UFUNCTION(BlueprintCallable, Category = "Custom")
	void activateOverviewMode();

private:
	/** Variables handling the player orbiting around a point */
	float currentXRotationAroundObject;
	float currentZRotationAroundObject;

	class UCameraFollow *cameraFollow;

	/** Handles the player's state */
	enum AppState
	{
		ORBIT,
		FREERUN,
		TOUR,
		PANORAMA
	};
	AppState state;

protected:
	// Helper function to reposition the player given the current orbit status
	void orbitReposition();

	// Handles tap and drag input within various states along the x axis
	void tapDragX(float deltaX);

	// Handles tap and drag input within various states along the y axis
	void tapDragY(float deltaY);

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
		TouchData() { bIsPressed = false; Location = FVector::ZeroVector; }
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

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};

