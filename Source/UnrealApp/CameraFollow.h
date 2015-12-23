#ifndef __CAMERAFOLLOW_H__
#define __CAMERAFOLLOW_H__

#include "UnrealAppCharacter.h"

class CameraFollow
{
public:
	CameraFollow(AUnrealAppCharacter *playerInput);


private:
	AUnrealAppCharacter *player;

};


#endif // __CAMERAFOLLOW_H__

