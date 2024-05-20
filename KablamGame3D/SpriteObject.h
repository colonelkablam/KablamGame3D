# pragma once

#include "GameConstants.h"
#include "Texture.h"

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
	int type;
	bool dead;
	bool illuminated;
	float timeElapsed;
	Texture* currentSprite;
	Texture* aliveSprite;
	Texture* deadSprite;

public:
	// default Constructor
	SpriteObject(float initX = 0.0f, float initY = 0.0f, float initZ = 0.0f, int initType = 0, bool isDead = false,
		bool isIlluminated = false, int spriteWidth = 32, int spriteHeight = 32, bool rotate = false, Texture* initSpriteAlive = nullptr, Texture* initSpriteDead = nullptr)
		: x(initX), y(initY), z(initZ), type(initType), dead(isDead), illuminated(isIlluminated), width{ spriteWidth }, height{ spriteHeight }, rotatable{ rotate },
			currentSprite{ initSpriteAlive }, aliveSprite(initSpriteAlive), deadSprite(initSpriteDead), distFromPlayer{ 1000.f }, angleToPlayer{ 0.0f }, facingAngle{ 0.0f },
				relativeAngle{ 0.0f }, baseZ{ initZ }, timeElapsed{ 0.0f }
	{}

	~SpriteObject()
	{
		//delete sprite handled by KablamGame3D;
	}

	void UpdateSprite(float timeStep, float playerX, float playerY)
	{
		// add elapsed time
		timeElapsed += timeStep;

		// calculate distance from player
		float dx = x - playerX;
		float dy = y - playerY;
		distFromPlayer =  sqrt(dx * dx + dy * dy);

		// Calculate the angle from the player to the sprite
		// angle from player feet
		angleToPlayer = atan2(dy, dx);

		// update sprite view
		if (rotatable)
		{
			// Calculate the relative angle (where it is looking rel. to player)
			relativeAngle = facingAngle - angleToPlayer;

			// normalise between 0 - 2*PI
			if (relativeAngle < 0)
				relativeAngle += 2 * PI;
			if (relativeAngle > 2*PI)
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
			}

		}
	}

	CHAR_INFO GetPixel(int x, int y) const
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

	float GetDistanceFromPlayer() const
	{
		return distFromPlayer;
	}

	float GetAngleToPlayer() const
	{
		return angleToPlayer;
	}

	int GetSpriteWidth() const
	{
		return width;
	}

	int GetSpriteHeight() const
	{
		return height;
	}

	float GetZ() const
	{
		return z;
	}

	bool GetIlluminated() const
	{
		return illuminated;
	}

private:
	void Bobbing()
	{
		z = baseZ + sinf(timeElapsed * BOBBING_SPEED) * BOBBING_HEIGHT;
	}

};
