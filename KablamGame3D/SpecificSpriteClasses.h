#pragma once

#include <cmath>

#include "IntermediateSpriteClasses.h"

class OctoSprite : public MovableSprite, public RotatableSprite, public DestroyableSprite, public AISprite, public Bobbable {

public:
    OctoSprite(float initX, float initY, float initZ, Texture* initAliveSprite, Texture* initDeadSprite, Texture* initHitSprite, int initAggression, int initFireRate, bool initIsDead = false )
        : SpriteObject(initX, initY, initZ, SpriteType::OCTO_TYPE, false, SPRITE_TEXTURE_WIDTH, SPRITE_TEXTURE_HEIGHT, initAliveSprite),
        
        // built sprite inheritable behaviours
        MovableSprite(1.5f, 10.0f, 0.0f, 0.03f, 0.2f),
        RotatableSprite(),
        DestroyableSprite(100.0f, initDeadSprite, initHitSprite, initIsDead), 
        AISprite(initAggression, initFireRate),
		Bobbable(2.0f, 1.15f) {}

	virtual ~OctoSprite() = default;


	void UpdateSprite(float timeStep, float playerX, float playerY, float playerTilt, std::list<SpriteObject*>& allSprites, const std::vector<int>& floorMap) override
	{
		
		UpdateTimeAndDistanceToPlayer(timeStep, playerX, playerY);
		UpdateRelativeAngleToPlayer();
		UpdateAI(timeStep);
		UpdateMovement(timeStep, floorMap);
		UpdateIfHit(timeStep);
		Bobbing();
	}

    void UpdateAI(const float timeStep) override {
		//UpdateAI(timeStep);
		if (relativeAngle < PI)
			RotateAntiClockwise();
		else
			RotateClockwise();
	}
};

class BulletSprite : public MovableSprite, public DestroyableSprite {

public:
	BulletSprite(float initX, float initY, float initZ, Texture* initAliveSprite, Texture* initHitSprite, float initAngle)
		: SpriteObject(initX, initY, initZ, SpriteType::BULLET_TYPE, false, SPRITE_TEXTURE_WIDTH, SPRITE_TEXTURE_HEIGHT, initAliveSprite),
		// built sprite inheritable behaviours
		MovableSprite(16.0f, 16.0f, initAngle, 0.0f, 0.01f),
		DestroyableSprite(1.0f, initHitSprite, initHitSprite, false) {}

	virtual ~BulletSprite() = default;

	void UpdateSprite(float timeStep, float playerX, float playerY, float playerTilt, std::list<SpriteObject*>& allSprites, const std::vector<int>& floorMap) override {
		UpdateTimeAndDistanceToPlayer(timeStep, playerX, playerY);
		UpdateIfHit(timeStep);
		if (!IsSpriteDying()) {
			UpdateMovement(timeStep, floorMap); // collision with walls and movement
			if (hitWall) {
				MakeDying();
			}
			else {
				CheckCollisions(allSprites);
			}
		}
	}

private:
	void CheckCollisions(std::list<SpriteObject*>& allSprites) {
		for (auto* other : allSprites) {
			if (other == this) continue; // Skip self

			DestroyableSprite* target = dynamic_cast<DestroyableSprite*>(other);
			if (target && IsCollidingWith(target)) {
				HandleCollision(target);
			}
		}
	}

	bool IsCollidingWith(SpriteObject* other) {
		return RayIntersectsCircle(other->GetX(), other->GetY(), other->GetCollisionBuffer());
	}

	bool RayIntersectsCircle(float cx, float cy, float radius) {
		float dx = velocityX;
		float dy = velocityY;
		float fx = x - cx;
		float fy = y - cy;

		float a = dx * dx + dy * dy;
		float b = 2 * (fx * dx + fy * dy);
		float c = (fx * fx + fy * fy) - radius * radius;

		float discriminant = b * b - 4 * a * c;
		if (discriminant < 0) {
			return false;
		}

		discriminant = sqrt(discriminant);

		float t1 = (-b - discriminant) / (2 * a);
		float t2 = (-b + discriminant) / (2 * a);

		if (t1 >= 0 && t1 <= 1) {
			return true;
		}

		if (t2 >= 0 && t2 <= 1) {
			return true;
		}

		return false;
	}

	void HandleCollision(DestroyableSprite* target) {
		// Handle the collision (e.g., mark both the bullet and the target as hit or dying)
		SetHit(true); // Mark the bullet as hit
		MakeDying(); // Mark the bullet as dying
		target->SetHit(true); // Mark the target as hit
		target->MakeDying(); // Mark the target as dying
	}
};

class FloorlampSprite : public Static {

public:
	FloorlampSprite(float initX, float initY, float initZ, Texture* initAliveSprite)
		: SpriteObject(initX, initY, initZ, SpriteType::FLOORLAMP_TYPE, true, SPRITE_TEXTURE_WIDTH, SPRITE_TEXTURE_HEIGHT, initAliveSprite),

		// built sprite inheritable behaviours
		Static() {}

	virtual ~FloorlampSprite() = default;


	void UpdateSprite(float timeStep, float playerX, float playerY, float playerTilt, std::list<SpriteObject*>& allSprites, const std::vector<int>& floorMap) override
	{
		UpdateTimeAndDistanceToPlayer(timeStep, playerX, playerY);
	}

};
