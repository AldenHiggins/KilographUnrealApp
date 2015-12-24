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
	AUnrealAppCharacter *player;
	
};
