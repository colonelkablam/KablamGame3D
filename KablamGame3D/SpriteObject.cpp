# pragma once

#include <cmath>

#include "SpriteObject.h"

SpriteObject::SpriteObject(float initX, float initY, float initZ, SpriteType initType, bool isIlluminated, int spriteWidth, int spriteHeight, Texture* initSprite, float initAngle)
	: x(initX), y(initY), z(initZ), baseZ(initZ), distToPlayer(1000.f), angleToPlayer(0.0f), width(spriteWidth), height(spriteHeight), type(initType), 
		illuminated(isIlluminated), currentSprite(initSprite), timeElapsed(0.0f), facingAngle(initAngle), collisionBuffer(0.0f) {}


// base pixel fetching method
CHAR_INFO SpriteObject::GetPixel(int x, int y) const
{
	return currentSprite->GetPixel(x, y);
}

// all sprites need this to be drawn appropriately
void SpriteObject::UpdateTimeAndDistanceToPlayer(float timeStep, float playerX, float playerY)
{
	// add elapsed time
	timeElapsed += timeStep;

	// calculate distance from player
	distToPlayer = GetDistanceFromOther(playerX, playerY);;

	// Calculate the angle from the player to the sprite
	// angle from player feet
	float dx = x - playerX;
	float dy = y - playerY;
	angleToPlayer = atan2(dy, dx);
}

float SpriteObject::GetDistanceFromPlayer() const
{
	return distToPlayer;
}

float SpriteObject::GetDistanceFromOther(float otherX, float otherY) const
{
	float dx = x - otherX;
	float dy = y - otherY;
	return sqrt(dx * dx + dy * dy);
}

float SpriteObject::GetAngleToPlayer() const
{
	return angleToPlayer;
}

int SpriteObject::GetSpriteWidth() const
{
	return width;
}

int SpriteObject::GetSpriteHeight() const
{
	return height;
}

float SpriteObject::GetX() const
{
	return x;
}
float SpriteObject::GetY() const
{
	return y;
}

float SpriteObject::GetZ() const
{
	return z;
}

bool SpriteObject::GetIlluminated() const
{
	return illuminated;
}

SpriteType SpriteObject::GetSpriteType() const
{
	return type;
}

float SpriteObject::GetCollisionBuffer() const
{
	return collisionBuffer;
}