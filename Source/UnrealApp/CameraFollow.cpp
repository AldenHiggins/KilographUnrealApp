// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealApp.h"
#include "CameraFollow.h"
#include "UnrealAppCharacter.h"


// Sets default values for this component's properties
UCameraFollow::UCameraFollow()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCameraFollow::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCameraFollow::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

void UCameraFollow::setPlayer(AUnrealAppCharacter *playerInput)
{
	player = playerInput;
}

void UCameraFollow::startFollowing()
{
	UE_LOG(Kilograph, Log, TEXT("STARTED CAMERA FOLLOWING WHOOOO"));
}

