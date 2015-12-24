#pragma once

#include "Components/ActorComponent.h"
#include "CameraFollow.generated.h"

class AUnrealAppCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALAPP_API UCameraFollow : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Set the player attached to this camera path
	void setPlayer(AUnrealAppCharacter *playerInput);

	// Start the camera following sequence
	void startFollowing();

	// Sets default values for this component's properties
	UCameraFollow();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

private:
	// The player that will be affected by this camera path
	AUnrealAppCharacter *player;

	// The elements of the camera path
	TArray<AActor *> cameraPathElements;

	// The current array index
	int currentIndex;

	// Determines whether the player is in follow mode or not
	bool followMode;
};
