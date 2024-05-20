# pragma once

#include "SpriteObject.h"

SpriteObject::SpriteObject(float initX, float initY, float initZ, int initType, bool isDead, bool isIlluminated, int spriteWidth, int spriteHeight, bool rotate, 
							Texture* initSpriteAlive, Texture* initSpriteDead, Texture* initSpriteHit, float direction)
		: x(initX), y(initY), z(initZ), type(initType), dead(isDead), illuminated(isIlluminated), width{ spriteWidth }, height{ spriteHeight }, rotatable{ rotate },
			currentSprite{ initSpriteAlive }, aliveSprite(initSpriteAlive), deadSprite(initSpriteDead), hitSprite{ initSpriteHit }, distFromPlayer { 1000.f }, angleToPlayer{ 0.0f },
				facingAngle{ direction }, relativeAngle{ 0.0f }, baseZ{ initZ }, timeElapsed{ 0.0f }, eventTimer{ 0.0f }, hit {false}
{}

SpriteObject::~SpriteObject()
{
	//delete sprite handled by KablamGame3D;
}

void SpriteObject::UpdateSprite(const float timeStep, const float playerX, const float playerY, const std::list<SpriteObject>& allSprites, const std::vector<int>)
{
	// add elapsed time
	timeElapsed += timeStep;

	// calculate distance from player
	distFromPlayer = GetDistanceFromOther(playerX, playerY);;

	// Calculate the angle from the player to the sprite
	// angle from player feet
	float dx = x - playerX;
	float dy = y - playerY;
	angleToPlayer = atan2(dy, dx);

	// update sprite view
	if (rotatable)
	{
		// Calculate the relative angle (where it is looking rel. to player)
		relativeAngle = facingAngle - angleToPlayer + PI;

		// normalise between 0 - 2*PI
		if (relativeAngle < 0)
			relativeAngle += 2 * PI;
		if (relativeAngle > 2 * PI)
			relativeAngle -= 2 * PI;

		if (type != 0)
		{
			if (relativeAngle > PI)
				facingAngle += 0.02;
			else
				facingAngle -= 0.02;

			// normalise between 0 - 2*PI
			if (facingAngle < 0)
				facingAngle += 2 * PI;
			if (facingAngle > 2 * PI)
				facingAngle -= 2 * PI;

			// bob the sprite
			Bobbing();

			if (GetSpriteObjectHitType(allSprites) == 3)
			{

			}

			if (hit)
			{
				currentSprite = hitSprite;
			}


		}
	}

	if (type == 4)
	{
		x += cosf(facingAngle) * timeStep * 50;
		y += sinf(facingAngle) * timeStep * 50;


	}
}

CHAR_INFO SpriteObject::GetPixel(int x, int y) const
{
	int xOffset = 0;
	int yOffset = 0;

	if (rotatable) // if rotatable
	{
		// Determine the view index
		int viewIndex = static_cast<int>((relativeAngle + SEGMENT_ANGLE / 2) / SEGMENT_ANGLE) % 8;

		// Calculate offsets based on the view index
		xOffset = (viewIndex % 4) * width; // 4 views per row
		yOffset = (viewIndex / 4) * height; // Move to next row for views 4-7
	}
	return currentSprite->GetPixel(x + xOffset, y + yOffset);
}

float SpriteObject::GetDistanceFromPlayer() const
{
	return distFromPlayer;
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

float SpriteObject::GetZ() const
{
	return z;
}

bool SpriteObject::GetIlluminated() const
{
	return illuminated;
}

int  SpriteObject::GetSpriteType() const
{
	return type;
}


void SpriteObject::Bobbing()
{
	z = baseZ + sinf(timeElapsed * BOBBING_SPEED) * BOBBING_HEIGHT;
}

int SpriteObject::GetSpriteObjectHitType(const std::list<SpriteObject>& allSprites)
{
	bool anyHit{ false };

	for (const SpriteObject& other : allSprites)
	{
		if (other.GetSpriteType() == 4) // 4 = playerFireball
		{
			if (GetDistanceFromOther(other.x, other.y) < 0.5f)
			{
				hit = true;
				eventTimer = timeElapsed;
				return 1;
			}
		}
	}
	return 0;
}

bool SpriteObject::CheckCollisionWithWall(const std::vector<int> wallMap)
{
	return false;
}

