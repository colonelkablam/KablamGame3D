#pragma once

#include <cmath>

#include "KablamGame3D.h"
#include "IntermediateSpriteClasses.h"

class Enemy : public Collidable, public MovableSprite, public RotatableAnimatable, public DestroyableSprite, public AISprite, public Bobbable {

public:
    Enemy(float initX, float initY, float initZ, Texture* initAliveSprite, Texture* initDeadSprite, Texture* initHitSprite, int initAggression, int initFireRate, bool initIsDead = false )
        : SpriteObject(initX, initY, initZ, SpriteType::OCTO_TYPE, false, SPRITE_TEXTURE_WIDTH, SPRITE_TEXTURE_HEIGHT, initAliveSprite),
        
        // built sprite inheritable behaviours
        MovableSprite(1.5f, 10.0f, 0.0f, 0.03f),
		Collidable(0.3f),
        RotatableAnimatable(),
        DestroyableSprite(100.0f, initDeadSprite, initHitSprite, initIsDead), 
        AISprite(initAggression, initFireRate),
		Bobbable(2.0f, 1.15f) {}

	virtual ~Enemy() = default;


	void UpdateSprite(float timeStep, float playerX, float playerY, float playerTilt, const std::vector<int>& floorMap, std::list<SpriteObject*>& allSprites) override
	{
		
		SpriteObject::UpdateTimeAndDistanceToPlayer(timeStep, playerX, playerY);
		RotatableAnimatable::UpdateRelativeAngleToPlayer();
		AISprite::UpdateAI(timeStep);
		MovableSprite::UpdateMovement(timeStep, floorMap, allSprites);
		DestroyableSprite::UpdateIfHit(timeStep);
		Bobbable::Bobbing();
	}

	// MovableSprite virtual function that needs overriding
	void UpdateMovement(float timeStep, const std::vector<int>& floorMap, std::list<SpriteObject*>& allSprites) override
	{
		MovableSprite::UpdateVelocity(timeStep); // update the velocity of sprite

		// update the hit flags
		Collidable::UpdateHitFlags(velocityX, velocityY, floorMap, allSprites);

		// if hitting another sprite no movement - just rotation from 'AI'
		if (hitOther) {
			return;
		}
		else // if hitting a wall add x or y movement accordingly - allows for wall 'sliding'
		{
			if (!hitWallX)
				x += velocityX;
			if (!hitWallY)
				y += velocityY;
		}
	}

	// UpdateAI virtual function that needs overriding
    void UpdateAI( float timeStep) override {
		//UpdateAI(timeStep);
		if (relativeAngle < PI)
			RotateAntiClockwise();
		else
			RotateClockwise();
	}
};

class BulletSprite : public Collidable, public MovableSprite, public DestroyableSprite {

public:
	BulletSprite(float initX, float initY, float initZ, Texture* initAliveSprite, Texture* initHitSprite, float initAngle)
		: SpriteObject(initX, initY, initZ, SpriteType::BULLET_TYPE, false, SPRITE_TEXTURE_WIDTH, SPRITE_TEXTURE_HEIGHT, initAliveSprite),
		// built sprite inheritable behaviours
		MovableSprite(16.0f, 16.0f, initAngle, 0.0f),
		Collidable(0.0f),
		DestroyableSprite(1.0f, initHitSprite, initHitSprite, false) {}

	virtual ~BulletSprite() = default;

	void UpdateSprite(float timeStep, float playerX, float playerY, float playerTilt, const std::vector<int>& floorMap, std::list<SpriteObject*>& allSprites) override {
		// update position relative to player
		SpriteObject::UpdateTimeAndDistanceToPlayer(timeStep, playerX, playerY);

		// see if dying
		if (DestroyableSprite::IsSpriteDying()) {
			DestroyableSprite::UpdateIfHit(timeStep); // update dying texture and time
		}
		else // else continue to update movement and state
		{
			MovableSprite::UpdateMovement(timeStep, floorMap, allSprites);
		}
	}

private:

	// MovableSprite virtual function that needs overriding
	void UpdateMovement(float timeStep, const std::vector<int>& floorMap, std::list<SpriteObject*>& allSprites) override
	{
		if (hitWallX || hitWallY)
		{
			DestroyableSprite::MakeDying();
			return;
		}
		else if (CheckCollisions(allSprites)){
			x += velocityX;
			y += velocityY;
		}
	}

	bool CheckCollisions(std::list<SpriteObject*>& allSprites) {
		for (auto* other : allSprites) {
			if (other == this) continue; // Skip self

			DestroyableSprite* destroyableTarget = dynamic_cast<DestroyableSprite*>(other);
			Collidable* collidableTarget = dynamic_cast<Collidable*>(other);

			if (destroyableTarget && collidableTarget && IsCollidingWith(collidableTarget)) {
				HandleCollision(destroyableTarget);
				return true;
			}
		}
		return false; // no collisions
	}

	bool IsCollidingWith(Collidable* other) {
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
		MakeHit(); // Mark the bullet as hit
		MakeDying(); // Mark the bullet as dying
		target->MakeHit(); // Mark the target as hit
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


	void UpdateSprite(float timeStep, float playerX, float playerY, float playerTilt, const std::vector<int>& floorMap, std::list<SpriteObject*>& allSprites) override
	{
		SpriteObject::UpdateTimeAndDistanceToPlayer(timeStep, playerX, playerY);
	}

};
