#include "FloorLampSprite.h"

FloorLampSprite::FloorLampSprite(float initX, float initY, float initZ, Texture* initAliveSprite)
	: SpriteObject(initX, initY, initZ, SpriteType::FLOORLAMP_TYPE, true, SPRITE_TEXTURE_WIDTH, SPRITE_TEXTURE_HEIGHT, initAliveSprite),
	// built sprite inheritable behaviours
	Static() {}

	void FloorLampSprite::UpdateSprite(float timeStep, float playerX, float playerY, float playerTilt, const std::vector<int>& floorMap, std::list<SpriteObject*>& allSprites)
	{
		SpriteObject::UpdateTimeAndDistanceToPlayer(timeStep, playerX, playerY);
	}

