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
}

// Called when the game starts
void UCameraFollow::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<USceneComponent*> childrenRoots;
	GetOwner()->GetRootComponent()->GetChildrenComponents(true, childrenRoots);

	for (int childIndex = 0; childIndex < childrenRoots.Num(); childIndex++)
	{
		cameraPathElements.Add(childrenRoots[childIndex]->GetOwner());
	}

	//UE_LOG(Kilograph, Log, TEXT("Number of path elements: %d"), cameraPathElements.Num());
}

// Called every frame
void UCameraFollow::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (!followMode)
	{
		return;
	}

	AActor *nextBox = cameraPathElements[currentIndex];

	FVector distanceVector = nextBox->GetActorLocation() - player->GetActorLocation();

	//UE_LOG(Kilograph, Log, TEXT("MAGNITUDE: %f"), distanceVector.Size());

	if (distanceVector.Size() < 100.0f)
	{
		currentIndex++;
		if (currentIndex >= cameraPathElements.Num())
		{
			currentIndex = 0;
			nextBox = cameraPathElements[currentIndex];
			distanceVector = nextBox->GetActorLocation() - player->GetActorLocation();
		}
	}

	distanceVector.Normalize();
	//UE_LOG(Kilograph, Log, TEXT("DISTANCE: %s"), *distanceVector.ToString());
	//UE_LOG(Kilograph, Log, TEXT("Movement amount: %f"), 1000.0f * DeltaTime);
	player->AddMovementInput(distanceVector, 1000.0f * DeltaTime);
}

void UCameraFollow::setPlayer(AUnrealAppCharacter *playerInput)
{
	player = playerInput;
}

void UCameraFollow::startFollowing()
{
	currentIndex = 0;
	player->SetActorLocation(GetOwner()->GetActorLocation());
	followMode = true;
	//UE_LOG(Kilograph, Log, TEXT("STARTED CAMERA FOLLOWING WHOOOO"));
}

void UCameraFollow::stopFollowing()
{
	followMode = false;
}



