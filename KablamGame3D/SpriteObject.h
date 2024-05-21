# pragma once

#include<Windows.h>
#include <list>
#include <vector>

#include "GameConstants.h"
#include "Texture.h"

#include "SpriteEnums.h"


class SpriteObject {

private:

	const float SEGMENT_ANGLE = (2 * PI) / 8; // Each segment covers 45 degrees or PI/4 radians
	const float BOBBING_SPEED = 4.5f; // Speed of the bobbing motion
	const float BOBBING_HEIGHT = 0.15f; // Amplitude of the bobbing motion

	float x;
	float y;
	float z; // height off floor
	float baseZ; // starting height
	float distFromPlayer;
	float angleToPlayer;
	float facingAngle;
	float relativeAngle;
	int width;
	int height;
	bool rotatable;
	SpriteType type;
	bool dead;
	bool hit;
	bool illuminated;
	float timeElapsed;
	float eventTimer;
	Texture* currentSprite;
	Texture* aliveSprite;
	Texture* hitSprite;
	Texture* deadSprite;

	float hitDisplayTime = 1.0f;


public:
	// default Constructor
	SpriteObject(float initX = 0.0f, float initY = 0.0f, float initZ = 0.0f, SpriteType initType = SpriteType::NO_TYPE, bool isDead = false, bool isIlluminated = false, int spriteWidth = 32, int spriteHeight = 32,
					bool rotate = false, Texture* initSpriteAlive = nullptr, Texture* initSpriteDead = nullptr, Texture* initSpriteHit = nullptr, float facingAngle = 0.0f);

	~SpriteObject();

	void UpdateSprite(const float timeStep, const float playerX, const float playerY, std::list<SpriteObject>& allSprites, const std::vector<int>);

	CHAR_INFO GetPixel(int x, int y) const;

	float GetDistanceFromPlayer() const;

	float GetDistanceFromOther(float otherX, float otherY) const;

	float GetAngleToPlayer() const;

	int GetSpriteWidth() const;

	int GetSpriteHeight() const;

	float GetZ() const;

	bool GetIlluminated() const;
	
	void MakeDead();

	SpriteType  GetSpriteType() const;

private:
	void Bobbing();

	void CheckCollisionWithOtherSprites(std::list<SpriteObject>& allSprites);

	bool CheckCollisionWithWall(const std::vector<int> wallMap);


};
