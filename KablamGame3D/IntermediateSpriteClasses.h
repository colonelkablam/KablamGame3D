#pragma once

#include <math.h>

class BulletSprite
    ;
#include "SpriteObject.h"

#include "GameConstants.h"


class Static : public virtual SpriteObject {

public:
    Static() {}
};

class Collideable : public virtual SpriteObject {


public:
    Collideable(float collisionBuffer = 0.0f) {}
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
    Texture* deadSprite;
    float deadTime; // Time since the sprite was declared dead
    const float dyingDisplayDuration = 0.2f; // Duration to display dead sprite in seconds


public:
    DestroyableSprite(float initHealth = 100.0f, Texture* initDeadSprite = nullptr, Texture* initHitSprite = nullptr, bool initIsDead = false)
        : health(initHealth), dead(initIsDead), dying(false), hit(false), aliveSprite(currentSprite), deadSprite(initDeadSprite), hitSprite(initHitSprite), deadTime(0.0f) {}

    bool IsSpriteDead() const { return dead; }
    void MakeDead() { dead = true; }
    
    bool IsSpriteDying() const { return dying; }
    void MakeDying() { dying = true; currentSprite = deadSprite; }

    bool IsSpriteHit() const { return hit; }
    void SetHit(bool value) { hit = value; currentSprite = hitSprite; }

    void UpdateIfHit(float deltaTime) {
        if (dying) {
            deadTime += deltaTime;
            if (deadTime >= dyingDisplayDuration) {
                MakeDead();
            }
        }
    }
};

//class MovableSprite : public virtual SpriteObject {
//protected:
//    float velocityX;
//    float velocityY;
//    float currentSpeed;
//    float maxSpeed;
//    float rotationSpeed;
//    bool hitWall; // flag for if sprite hits wall in its update
//
//public:
//    MovableSprite(float initSpeed = 0.0f, float initMaxSpeed = 1.0f, float initAngle = 0.0f, float initRotationSpeed = 0.1f, float collisionBuffer = 0.0f)
//        : currentSpeed(initSpeed), maxSpeed(initMaxSpeed), rotationSpeed(initRotationSpeed), velocityX(0.0f), velocityY(0.0f), hitWall(false)
//    {
//        SpriteObject::facingAngle = initAngle; // reset facing angle
//        SpriteObject::collisionBuffer = collisionBuffer; // reset collision buffer as default is 0.0f
//    }
//
//    void UpdateMovement(float timeStep, const std::vector<int>& wallMap, int mapWidth, int mapHeight, std::list<SpriteObject*>& allSprites) {
//        hitWall = false; // reset hit wall
//
//        // Predictive movement
//        float predictedX = x + sinf(facingAngle + P2) * currentSpeed * timeStep;
//        float predictedY = y - cosf(facingAngle + P2) * currentSpeed * timeStep;
//
//        // Check collision with walls
//        int oldX = static_cast<int>(x);
//        int oldY = static_cast<int>(y);
//
//        // set appropriate collision buffer sign
//        float xBuff = velocityX > 0 ? collisionBuffer : -collisionBuffer;
//        float yBuff = velocityY > 0 ? collisionBuffer : -collisionBuffer;
//
//        int newX = static_cast<int>(predictedX + xBuff);
//        int newY = static_cast<int>(predictedY + yBuff);
//
//        // Check for wall collisions
//        if (newX >= 0 && newX < mapWidth && wallMap[oldY * mapWidth + newX] != 0)
//            hitWall = true;
//
//        if (newY >= 0 && newY < mapHeight && wallMap[newY * mapWidth + oldX] != 0)
//            hitWall = true;
//
//        // Check for collisions with other movable sprites
//        bool collisionWithOtherSprite = false;
//        for (auto& other : allSprites) {
//            if (other == this) continue; // Skip self
//
//            MovableSprite* movable = dynamic_cast<MovableSprite*>(other);
//            if (movable && IsCollidingWith(predictedX, predictedY, movable)) {
//                collisionWithOtherSprite = true;
//                break;
//            }
//        }
//
//        // Only update position if no collision
//        if (!hitWall && !collisionWithOtherSprite) {
//            x = predictedX;
//            y = predictedY;
//        }
//    }
//
//    void IncreaseSpeed(float amount) {
//        currentSpeed += amount;
//        if (currentSpeed > maxSpeed) {
//            currentSpeed = maxSpeed; // Clamp to maxSpeed
//        }
//    }
//
//    void DecreaseSpeed(float amount) {
//        currentSpeed -= amount;
//        if (currentSpeed < 0) {
//            currentSpeed = 0; // Clamp to zero
//        }
//    }
//
//    void RotateClockwise() {
//        facingAngle += rotationSpeed;
//
//        // Normalize facing angle below 2*PI
//        if (facingAngle >= 2 * PI) facingAngle -= 2 * PI;
//    }
//
//    void RotateAntiClockwise() {
//        facingAngle -= rotationSpeed;
//
//        // Normalize facing angle above 0
//        if (facingAngle < 0) facingAngle += 2 * PI;
//    }
//
//    float GetVelocityX() const { return velocityX; }
//    float GetVelocityY() const { return velocityY; }
//
//private:
//    bool IsCollidingWith(float predictedX, float predictedY, MovableSprite* other) {
//        float dx = predictedX - other->GetX();
//        float dy = predictedY - other->GetY();
//        float distance = sqrtf(dx * dx + dy * dy);
//        return distance < (width / 2 + other->GetSpriteWidth() / 2);
//    }
//};

class MovableSprite : public virtual SpriteObject {
protected:
    float velocityX;
    float velocityY;
    float currentSpeed;
    float maxSpeed;
    float rotationSpeed;
    bool hitOther;
    bool hitWall; // flag for if sprite hits wall in its update

public:
    MovableSprite(float initSpeed = 0.0f, float initMaxSpeed = 1.0f, float initAngle = 0.0f, float initRotationSpeed = 0.1f, float collisionBuffer = 0.0f)
        : currentSpeed(initSpeed), maxSpeed(initMaxSpeed), rotationSpeed(initRotationSpeed), velocityX(0.0f), velocityY(0.0f), hitWall(false)
    {
        SpriteObject::facingAngle = initAngle; // reset facing angle
        SpriteObject::collisionBuffer = collisionBuffer; // reset collision buffer as default is 0.0f
    }

    void UpdateMovement(float timeStep, std::vector<int> wallMap, const std::list<SpriteObject*>& spriteObjects) {

        hitWall = false; // reset hit wall

        // Update velocity based on speed and facing angle
        velocityX = sinf(facingAngle + P2) * currentSpeed * timeStep;
        velocityY = -cosf(facingAngle + P2) * currentSpeed * timeStep;

        // Check collision with walls and adjust velocity if needed
        int oldX = static_cast<int>(x);
        int oldY = static_cast<int>(y);

        // set appropriate collision buffer sign
        float xBuff = velocityX > 0 ? collisionBuffer : -collisionBuffer;
        float yBuff = velocityY > 0 ? collisionBuffer : -collisionBuffer;

        int newX = static_cast<int>(x + velocityX + xBuff);
        int newY = static_cast<int>(y + velocityY + yBuff);

        bool bumpedOtherX{ false };
        bool bumpedOtherY{ false };


        for (const auto& sprite : spriteObjects)
        {
            // Use dynamic cast to check if the sprite is a MovableSprite
            MovableSprite* movable = dynamic_cast<MovableSprite*>(sprite);

            if (movable) {
                // Skip self in collision check
                if (movable == this)
                    continue;

                // Check for collision in the x direction
                if (movable->GetDistanceFromOther(newX, y) < movable->GetCollisionBuffer()) {
                    bumpedOtherX = true;
                }

                // Check for collision in the y direction
                if (movable->GetDistanceFromOther(x, newY) < movable->GetCollisionBuffer()) {
                    bumpedOtherY = true;
                }

                // If collision in both directions, break early
                if (bumpedOtherX && bumpedOtherY)
                    break;
            }
        }

        // Update position based on velocity if no wall - hitwall true
        if (newX >= 0 && newX < MAP_WIDTH && wallMap[oldY * MAP_WIDTH + newX] == 0 && !bumpedOtherX )
            x += velocityX;
        else
            hitWall = true;

        if (newY >= 0 && newY < MAP_HEIGHT && wallMap[newY * MAP_WIDTH + oldX] == 0 && !bumpedOtherY)
            y += velocityY;
        else
            hitWall = true;
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


class AISprite : public virtual SpriteObject {
protected:
    int aggression;
    int fireRate;
public:
    AISprite(int initAggression = 0.0f, int initFireRate = 10)
        : aggression (initAggression), fireRate(initFireRate) {}

    // to be defined by particular AI of specific sprite
    virtual void UpdateAI(const float timeStep) = 0;
};