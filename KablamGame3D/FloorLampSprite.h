#pragma once
#include "IntermediateSpriteClasses.h"

class FloorLampSprite : public Static {

public:
	FloorLampSprite(float initX, float initY, float initZ, Texture* initAliveSprite);

	virtual ~FloorLampSprite() = default;

	void UpdateSprite(float timeStep, float playerX, float playerY, float playerTilt, const std::vector<int>& floorMap, std::list<SpriteObject*>& allSprites) override;

};

