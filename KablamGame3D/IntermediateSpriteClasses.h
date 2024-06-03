#pragma once

#include <math.h>

#include "SpriteObject.h"
#include "GameConstants.h"

class Static : public virtual SpriteObject {

public:
    Static() {}
};

class Bobbable : public virtual SpriteObject {
protected:
    float bobbingSpeed; // Speed of the bobbing motion
    float bobbingHeight; // Amplitude of the bobbing motion
public:
    Bobbable(int initBobbingSpeed = 4.0f, int initBobbingHeight = 1.0f)
        : bobbingSpeed(initBobbingSpeed), bobbingHeight(initBobbingHeight) {}

    void Bobbing()
    {
        z = baseZ + sinf(timeElapsed * bobbingSpeed) * bobbingHeight;
    }
};

class RotatableSprite : public virtual SpriteObject {
protected:
    // Each segment covers 45 degrees or PI/4 radians to handle texture mapping of different views
    const float SEGMENT_ANGLE = (2 * PI) / 8; 
    float relativeAngle;

public:
    RotatableSprite()
        : relativeAngle(0.0f) {}

    void UpdateRelativeAngleToPlayer()
    {
        // Calculate the relative angle (where it is looking rel. to player)
        relativeAngle = facingAngle - angleToPlayer + PI;
        
        // normalise between 0 - 2*PI
        if (relativeAngle < 0)
            relativeAngle += 2 * PI;
        if (relativeAngle > 2 * PI)
            relativeAngle -= 2 * PI;
    }

    CHAR_INFO GetPixel(int x, int y) const override {
        int xOffset = 0;
        int yOffset = 0;

        // Determine the view index
        int viewIndex = static_cast<int>((relativeAngle + SEGMENT_ANGLE / 2) / SEGMENT_ANGLE) % 8;

        // Calculate offsets based on the view index
        xOffset = (viewIndex % 4) * width; // 4 views per row
        yOffset = (viewIndex / 4) * height; // Move to next row for views 4-7
        
        if (currentSprite == nullptr)
            return { PIXEL_SOLID, FG_BLUE };
        else
            return currentSprite->GetPixel(x + xOffset, y + yOffset);
    }
};

class DestroyableSprite : public virtual SpriteObject {
protected:
    float health;
    bool hit; // flag if taken damage/hit during collision detection
    bool dying;
    bool dead;
    Texture* aliveSprite;
    Texture* hitSprite;
    Texture* dyingSprite;
    Texture* deadSprite;
    float hitTime;
    float dyingTime; // Time since the sprite was declared dead
    const float hitDisplayDuration = 0.3f; // Duration to display hit sprite in seconds
    const float dyingDisplayDuration = 0.3f; // Duration to display dead sprite in seconds


public:
    DestroyableSprite(float initHealth = 100.0f, Texture* initHitSprite = nullptr, Texture* initDyingSprite = nullptr, Texture* initDeadSprite = nullptr, bool initIsDead = false)
        : health(initHealth), dead(initIsDead), dying(false), hit(false), aliveSprite(currentSprite), hitSprite(initHitSprite), dyingSprite(initDyingSprite), deadSprite(initDeadSprite),  hitTime(0.0f), dyingTime(0.0f) {}

    bool IsSpriteDead() const { return dead; }
    void MakeDead() {
        currentSprite = deadSprite;
        dead = true;
        z = 0.0f;
    }
    bool IsSpriteDying() const { return dying; }
    void MakeDying() { dying = true; currentSprite = deadSprite; }

    void SetDamage(float amount)
    {
        health -= amount;
    }

    bool IsSpriteHit() const { return hit; }
    void SetHit(bool value) { hit = value; currentSprite = hitSprite; }

    void UpdateIfHit(float deltaTime) {
        if (hit)
        {
            hitTime += deltaTime;

            if (currentSprite != hitSprite)
                currentSprite = hitSprite;

            if (hitTime >= hitDisplayDuration) {
                currentSprite = aliveSprite;
                hit = false;
            }
        }

        if ( health < 0.0f || dead) {
            dyingTime += deltaTime;

            if (currentSprite != dyingSprite)
                currentSprite = deadSprite;

            if (dyingTime >= dyingDisplayDuration) {
                MakeDead();
            }
        }

        //// see if dying
        //if (IsSpriteDying()) {
        //    DestroyableSprite::UpdateIfHit(timeStep); // update dying texture and time
        //}
        //else // else continue to update movement and state
        //{
        //    if (MovableSprite::hitWall) {
        //        DestroyableSprite::MakeDying();
        //    }
        //    else {
        //        MovableSprite::UpdateMovement(timeStep, floorMap, allSprites); // collision with walls and movement
        //        //CheckCollisions(allSprites);
        //    }
        //}
    }
};

class MovableSprite : public virtual SpriteObject {
protected:
    float velocityX;
    float velocityY;
    float currentSpeed;
    float maxSpeed;
    float rotationSpeed;


public:
    MovableSprite(float initSpeed = 0.0f, float initMaxSpeed = 1.0f, float initAngle = 0.0f, float initRotationSpeed = 0.1f)
        : currentSpeed(initSpeed), maxSpeed(initMaxSpeed), rotationSpeed(initRotationSpeed), velocityX(0.0f), velocityY(0.0f)
    {
        SpriteObject::facingAngle = initAngle; // reset facing angle
    }

    // virtual movement that needs overriding by child class
    virtual void UpdateMovement(float timeStep, const std::vector<int>& floorMap, std::list<SpriteObject*>& allSprites) = 0 {};

    // update vector xy
    void UpdateVelocity(float timeStep)
    {
        UpdateVelocityX(timeStep);
        UpdateVelocityY(timeStep);
    }

    // only update x component of vector (needed for wall sliding)
    void UpdateVelocityX(float timeStep) {
        // Update velocity X-axis based on speed and facing angle
        velocityX = sinf(facingAngle + P2) * currentSpeed * timeStep;
    }

    float GetVelocityX()
    {
        return velocityX;
    }

    // only update y component of vector (needed for wall sliding)
    void UpdateVelocityY(float timeStep) {
        // Update velocity Y-axis based on speed and facing angle
        velocityY = -cosf(facingAngle + P2) * currentSpeed * timeStep;
    }

    float GetVelocityY()
    {
        return velocityY;
    }

    void IncreaseSpeed(float amount) {
        currentSpeed += amount; 
        if (currentSpeed > maxSpeed) {
            currentSpeed = maxSpeed; // Clamp to maxSpeed
        }
    }

    void DecreaseSpeed(float amount) {
        currentSpeed -= amount;
        if (currentSpeed < 0) {
            currentSpeed = 0; // Clamp to zero
        }
    }

    void RotateClockwise() {
        facingAngle += rotationSpeed;

        // Normalize facing angle below 2*PI
        if (facingAngle >= 2 * PI) facingAngle -= 2 * PI;
    }
    void RotateAntiClockwise() {
        facingAngle -= rotationSpeed;

        // Normalize facing angle above 0
        if (facingAngle < 0) facingAngle += 2 * PI;
    }

    float GetVelocityX() const { return velocityX; }
    float GetVelocityY() const { return velocityY; }
};

class Collidable : public virtual SpriteObject {
protected:
    float collisionBuffer; // effective physical size in game from center-line

    // flags for if sprite collisions
    bool hitOther;
    bool hitWallX;
    bool hitWallY;

public:
    Collidable(float collisionBuffer = 0.0f)
        : collisionBuffer(collisionBuffer), hitOther(false), hitWallX(false), hitWallY(false) {}


    void UpdateHitFlags(float vX, float vY, std::vector<int> wallMap, const std::list<SpriteObject*>& spriteObjects) {

        // reset hit flags
        hitWallX = false;
        hitWallY = false;
        hitOther = false;

        // Check collision with walls and adjust velocity if needed
        int oldX = static_cast<int>(x);
        int oldY = static_cast<int>(y);

        // set appropriate collision buffer sign
        float xBuff = vX > 0 ? collisionBuffer : -collisionBuffer;
        float yBuff = vY > 0 ? collisionBuffer : -collisionBuffer;

        int newX = static_cast<int>(x + vX + xBuff);
        int newY = static_cast<int>(y + vY + yBuff);

        for (const auto& sprite : spriteObjects)
        {
            // Use dynamic cast to check if the sprite is a MovableSprite
            Collidable* solidOther = dynamic_cast<Collidable*>(sprite);

            if (solidOther) {
                // Skip self in collision check
                if (solidOther == this)
                    continue;

                // Check for collision if moves new velcocity
                if (solidOther->GetDistanceFromOther(newX, newY) < solidOther->GetCollisionBuffer() + collisionBuffer) {
                    hitOther = true;
                }

                // If collision, break early, no need to check rest of list
                if (hitOther)
                    break;
            }
        }

        // Update position based on velocity if no wall - hitwall true
        if (newX >= 0 && newX < MAP_WIDTH && wallMap[oldY * MAP_WIDTH + newX] != 0)
            hitWallX = true;

        if (newY >= 0 && newY < MAP_HEIGHT && wallMap[newY * MAP_WIDTH + oldX] != 0)
            hitWallY = true;
    }

    float GetCollisionBuffer() const
    {
        return collisionBuffer;
    }

};


class AISprite : public virtual SpriteObject {
protected:
    int aggression;
    int fireRate;
public:
    AISprite(int initAggression = 0.0f, int initFireRate = 10)
        : aggression (initAggression), fireRate(initFireRate) {}

    // to be defined by particular AI of specific sprite
    virtual void UpdateAI(float timeStep) = 0 {};
};