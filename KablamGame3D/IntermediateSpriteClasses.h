#pragma once

#include <math.h>

#include "SpriteObject.h"
#include "GameConstants.h"
#include "SoundManager.h"

class Static : public virtual SpriteObject {

public:
    Static() {}
};

class MakesNoise : public virtual SpriteObject {
protected:
    SoundManager* soundManager;
public:
    MakesNoise(SoundManager* sound)
        : soundManager(sound) {}

    ~MakesNoise()
    {
       // delete soundManager;
    }

};

class DontCleanUpWhenDead : public virtual SpriteObject {

public:
    DontCleanUpWhenDead() {}
};

class Bobbable : public virtual SpriteObject {
protected:
    float bobbingSpeed; // Speed of the bobbing motion
    float bobbingHeight; // Amplitude of the bobbing motion
    bool isBobbing; // Flag to indicate if bobbing is active

public:
    Bobbable(float initBobbingSpeed = 3.0f, float initBobbingHeight = 1.0f)
        // generate a variation in bobbing speed with x/y location in starting array
        : bobbingSpeed(initBobbingSpeed + (1 - 1/(x+y/2) * (MAP_WIDTH + MAP_HEIGHT)/2 )), bobbingHeight(initBobbingHeight), isBobbing(true) {}

    void Bobbing()
    {
        if (isBobbing) {
            z = baseZ + sinf(timeElapsed * bobbingSpeed) * bobbingHeight;
        }
    }

    void StartBobbing() {
        isBobbing = true;
    }

    void StopBobbing() {
        isBobbing = false;
        z = baseZ; // Reset to the base position when bobbing stops
    }
};

class RotatableAnimatable : public virtual SpriteObject {
protected:
    // Each segment covers 45 degrees or PI/4 radians to handle texture mapping of different views
    const float SEGMENT_ANGLE = (2 * PI) / NUM_ROTATIONAL_VIEWING_SEGMENTS; 
    float relativeAngle;
    bool rotate;
    bool animate;
    int frameIndex;

public:
    RotatableAnimatable()
        : relativeAngle(0.0f), rotate(true), animate(false), frameIndex(0) {}

    void UpdateRelativeAngleToPlayer()
    {
        // Calculate the relative angle (where it is looking rel. to player)
        relativeAngle = facingAngle - angleFromPlayer + PI;
        
        // normalise between 0 - 2*PI
        if (relativeAngle < 0)
            relativeAngle += 2 * PI;
        if (relativeAngle > 2 * PI)
            relativeAngle -= 2 * PI;
    }
    void MakeRotatable()
    {
        animate = false;
        rotate = true;
    }

    void MakeAnimatable()
    {
        animate = true;
        rotate = false;
    }

    void MakeStatic()
    {
        animate = false;
        rotate = false;
    }

    void SetFrame(int index)
    {
        frameIndex = index % NUM_DYING_ANIMATION_FRAMES;
    }

    CHAR_INFO GetPixel(int x, int y) const override {
        if (currentSprite == nullptr) {
            return { PIXEL_SOLID, FG_BLUE }; // if no texture given
        }
        else if (rotate) { // rotatable display of texture
            int xOffset = 0;
            int yOffset = 0;

            int viewIndex = static_cast<int>((relativeAngle + SEGMENT_ANGLE / 2) / SEGMENT_ANGLE) % NUM_ROTATIONAL_VIEWING_SEGMENTS;

            int modulo = NUM_ROTATIONAL_VIEWING_SEGMENTS / 2;

            xOffset = (viewIndex % modulo) * width;
            yOffset = (viewIndex / modulo) * height;

            return currentSprite->GetPixel(x + xOffset, y + yOffset);
        }
        else if (animate) { // animation display (used when dying)

            int modulo = NUM_DYING_ANIMATION_FRAMES / 2;

            int xOffset = (frameIndex % modulo) * width;
            int yOffset = static_cast<int>(frameIndex / modulo) * height;
            return currentSprite->GetPixel(x + xOffset, y + yOffset);
        }
        else {
            return currentSprite->GetPixel(x, y);
        }
    }

    // linear interpolation pixel fetching method
    CHAR_INFO GetPixelLinearInterpolation(float x, float y) const
    {
        if (currentSprite == nullptr) {
            return { PIXEL_SOLID, FG_BLUE };
        }

        if (rotate) {
            // Calculate offsets based on the view index and the relative angle
            int viewIndex = static_cast<int>((relativeAngle + SEGMENT_ANGLE / 2) / SEGMENT_ANGLE) % NUM_ROTATIONAL_VIEWING_SEGMENTS;
            int modulo = NUM_ROTATIONAL_VIEWING_SEGMENTS / 2;

            int xOffset = (viewIndex % modulo) * width;
            int yOffset = (viewIndex / modulo) * height;

            //return currentSprite->LinearInterpolationWithGlyphShading(x + xOffset, y + yOffset);


            //// Adjust x and y with the calculated offsets
            x = (x * width + xOffset) / static_cast<float>(currentSprite->GetWidth());
            y = (y * height + yOffset) / static_cast<float>(currentSprite->GetHeight());
        }
        else if (animate) {
            // Calculate offsets based on the frame index
            int modulo = NUM_DYING_ANIMATION_FRAMES / 2;

            int xOffset = (frameIndex % modulo) * width;
            int yOffset = static_cast<int>(frameIndex / modulo) * height;

            //return currentSprite->LinearInterpolationWithGlyphShading(x + xOffset, y + yOffset);


            //// Adjust x and y with the calculated offsets
            x = (x * width + xOffset) / static_cast<float>(currentSprite->GetWidth());
            y = (y * height + yOffset) / static_cast<float>(currentSprite->GetHeight());
        }
        
        return currentSprite->LinearInterpolationWithGlyphShading(x, y);
    }

};

class DestroyableSprite : public virtual SpriteObject {
protected:
    float health;
    bool hit; // flag if taken damage/hit during collision detection
    bool dying;
    bool dead;
    bool removable;
    Texture* aliveSprite;
    Texture* hitSprite;
    Texture* dyingSprite;
    Texture* deadSprite;
    float hitTime;
    float dyingTime; // Time since the sprite was declared dead
    float deathTime;
    float hitDisplayDuration; // Duration to display hit sprite in seconds
    float dyingDisplayDuration; // Duration to display dead sprite in seconds
    float deadDisplayDuration; // Duration to display dead sprite in seconds


public:
    DestroyableSprite(float initHealth = 100.0f, Texture* initHitSprite = nullptr, Texture* initDyingSprite = nullptr, Texture* initDeadSprite = nullptr, float hitT = 0.3f, float dyingT = 1.0f, float deadT = 10.0f, bool initIsDead = false)
        : health(initHealth), dead(initIsDead), dying(false), hit(false), removable(false), aliveSprite(currentSprite), hitSprite(initHitSprite), dyingSprite(initDyingSprite), deadSprite(initDeadSprite),
            hitTime(0.0f), dyingTime(0.0f), deathTime(0.0f), hitDisplayDuration(hitT), dyingDisplayDuration(dyingT), deadDisplayDuration(deadT) {
        currentSprite = aliveSprite; // Ensure the initial sprite is set correctly
    }

    bool IsSpriteDead() const { return dead; }

    void MakeDead() {
        currentSprite = deadSprite;
        deathTime = 0.0f;
        dead = true;
        dying = false;
        hit = false;
    }

    bool IsSpriteDying() const { return dying; }

    void MakeDying() {
        dying = true;
        hit = false;
        dead = false;
        currentSprite = dyingSprite;
        dyingTime = 0.0f;
    }

    bool IsSpriteHit() const { return hit; }

    void MakeHit() {
        if (!dead && !dying) {
            hit = true;
            currentSprite = hitSprite;
            hitTime = 0.0f; // Reset hit time when hit
        }
    }

    void MakeUnHit() {
        hit = false;
        hitTime = 0.0f;
        currentSprite = aliveSprite;
    }

    bool IsRemovable()
    {
        return removable;
    }

    void SetDamage(float amount)
    {
        if (!dead && !dying) {
            health -= amount;
            if (health <= 0.0f && !dead && !dying)
            {
                RotatableAnimatable* rotateable = dynamic_cast<RotatableAnimatable*>(this);
                if (rotateable)
                {
                    rotateable->MakeStatic();
                }

                MakeDying();
            }
            else if (!hit) {
                MakeHit();
            }
        }
    }

    void UpdateIfHit(float deltaTime) {
        RotatableAnimatable* animatable = dynamic_cast<RotatableAnimatable*>(this);

        if (dead) {
            deathTime += deltaTime;
            if (deathTime >= deadDisplayDuration)
                removable = true;

            return;
        }

        // if a rotatable and animatable sprite
        if (animatable && dying) {
            animatable->MakeAnimatable();

            dyingTime += deltaTime;

            // Calculate frame duration for the dying animation
            float frameDuration = dyingDisplayDuration / NUM_DYING_ANIMATION_FRAMES; // frames in animation texture
            int frameIndex = static_cast<int>(dyingTime / frameDuration);

            // Ensure frame index is within valid range
            if (frameIndex >= NUM_DYING_ANIMATION_FRAMES) {
                animatable->MakeStatic();
                MakeDead();
            }
            else {
                // Update the current sprite based on the dying frame
                animatable->SetFrame(frameIndex);
            }
        }
        else if (dying) {
            dyingTime += deltaTime;

            if (dyingTime >= dyingDisplayDuration) {
                MakeDead();
            }
        }
        else if (hit) {
            hitTime += deltaTime;

            if (hitTime >= hitDisplayDuration) {
                MakeUnHit();
            }
        }
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

    void SetSpeed(float amount)
    {
        currentSpeed = amount;
    }

    void SetRotationSpeed(float amount)
    {
        rotationSpeed = amount;
    }

    void RotateClockwise(float timeStep) {
        facingAngle += rotationSpeed * timeStep;;

        // Normalize facing angle below 2*PI
        if (facingAngle >= 2 * PI) facingAngle -= 2 * PI;
    }
    void RotateAntiClockwise(float timeStep) {
        facingAngle -= rotationSpeed * timeStep;

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

        // find new xy coords if move made
        float fNewX = x + vX + xBuff;
        float fNewY = y + vY + yBuff;

        // get new xy of map
        int newX = static_cast<int>(x + vX + xBuff);
        int newY = static_cast<int>(y + vY + yBuff);

        // Update position based on velocity if no wall - hitwall true

        if (oldY * MAP_WIDTH + newX < wallMap.size() || newY * MAP_WIDTH + oldX < wallMap.size()) // with wallMap array
        {
            if (newX >= 0 && newX < MAP_WIDTH && wallMap[oldY * MAP_WIDTH + newX] != 0)
                hitWallX = true;

            if (newY >= 0 && newY < MAP_HEIGHT && wallMap[newY * MAP_WIDTH + oldX] != 0)
                hitWallY = true;
        }

        for (const auto& sprite : spriteObjects)
        {
            // Use dynamic cast to check if the sprite is a Collidable sprite
            Collidable* solidOther = dynamic_cast<Collidable*>(sprite);

            if (solidOther) {
                // Skip self in collision check
                if (solidOther == this)
                    continue;

                // Use dynamic cast to check if the sprite is a Destroyable (as will pass through dying and dead sprites)
                DestroyableSprite* destroyable = dynamic_cast<DestroyableSprite*>(solidOther);
                bool deadOrDying{ false };
                if (destroyable) {
                    deadOrDying = destroyable->IsSpriteDying() || destroyable->IsSpriteDead();
                }
                // Check for collision if sprite moves the new velocity
                if (deadOrDying == false && solidOther->GetDistanceFromOther(fNewX, fNewY) < (solidOther->GetCollisionBuffer() + collisionBuffer)) {
                    hitOther = true;
                    break; // If collision, break early, no need to check rest of list
                }
            }
        }

    }

    float GetCollisionBuffer() const
    {
        return collisionBuffer;
    }

};


class AISprite : public virtual SpriteObject {
protected:
    bool isFacingPlayer;
    bool isWallInWay;
    bool canSeePlayer;
    bool knowsWherePlayerWas;
    float playerWasX;
    float playerWasY;
    float timeToForget; // currently set to a default in constructor
    int aggression;
    int fireRate;

public:
    AISprite(int initAggression = 0.0f, int initFireRate = 10)
        : aggression (initAggression), fireRate(initFireRate), isFacingPlayer(false), isWallInWay(false), canSeePlayer(false), knowsWherePlayerWas(false),
        playerWasX (1.0f), playerWasY(0.0f), timeToForget(5.0f) {}

    // to be defined by particular AI of specific sprite
    virtual void UpdateAI(float timeStep) = 0 {};

    void UpdateCanSpriteSeePlayer(const std::vector<int>& mapWalls, float playerX, float playerY) {
        // Check if the sprite is facing the player

        float pA{ angleFromPlayer };
        pA += PI;
        if (pA > PI2)
            pA -= PI2;

        isFacingPlayer = fabs(facingAngle - pA) < PI/2; // Adjust the angle tolerance as needed

        if (!isFacingPlayer) {
            canSeePlayer = false;
            return;
        }

        // Get the starting position of the sprite
        float x0 = GetX();
        float y0 = GetY();
        float x1 = playerX;
        float y1 = playerY;

        // Calculate the direction vector from the sprite to the player
        float dx = x1 - x0;
        float dy = y1 - y0;
        float distance = sqrt(dx * dx + dy * dy);
        float stepSize = 0.1f; // The size of each step along the line

        // Normalize the direction vector
        dx /= distance;
        dy /= distance;

        float currentX = x0;
        float currentY = y0;

        while (true) {
            // Calculate the current position in the map grid
            int mapX = static_cast<int>(currentX);
            int mapY = static_cast<int>(currentY);

            // Check if the current position is within bounds
            if (mapX >= 0 && mapX < MAP_WIDTH && mapY >= 0 && mapY < MAP_HEIGHT) {
                // Calculate the index in the 1D mapWalls vector
                int index = mapY * MAP_WIDTH + mapX;

                // Check if there's a wall
                if (mapWalls[index] > 0) {
                    isWallInWay = true;
                    canSeePlayer = false;
                    return;
                }
            }
            else {
                canSeePlayer = false;
                isWallInWay = true;
                return; // Out of bounds
            }

            // Check if we've reached the end point
            if (sqrt((currentX - x1) * (currentX - x1) + (currentY - y1) * (currentY - y1)) < stepSize) {
                canSeePlayer = true;
                isWallInWay = false;
                return;
            }

            // Move along the line by the step size
            currentX += dx * stepSize;
            currentY += dy * stepSize;
        }

        canSeePlayer = false;
        isWallInWay = false;
    }

};