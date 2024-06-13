#include "Enemy.h"

Enemy::Enemy(float initX, float initY, float initZ, Texture* initAliveSprite, Texture* initHitSprite, Texture* initDyingSprite, Texture* initDeadSprite, int initAggression, int initFireRate, bool initIsDead, SoundManager* sounds)
    : onDeathAddScoreCallback(nullptr),
    scoreCounted(false),
    SpriteObject(initX, initY, initZ, SpriteType::OCTO_TYPE, false, SPRITE_TEXTURE_WIDTH, SPRITE_TEXTURE_HEIGHT, initAliveSprite),
    MovableSprite(1.5f, 10.0f, 0.0f, 1.0f),
    Collidable(0.3f),
    RotatableAnimatable(),
    DestroyableSprite(100.0f, initHitSprite, initDyingSprite, initDeadSprite, 0.3f, 1.0f, 10.0f, initIsDead),
    AISprite(initAggression, initFireRate),
    Bobbable(2.0f, 0.6f),
    MakesNoise(sounds) {}

void Enemy::UpdateSprite(float timeStep, float playerX, float playerY, float playerTilt, const std::vector<int>& environmentMap, std::list<SpriteObject*>& allSprites) {
    SpriteObject::UpdateTimeAndDistanceToPlayer(timeStep, playerX, playerY);
    RotatableAnimatable::UpdateRelativeAngleToPlayer();
    Enemy::UpdateAI(timeStep);
    AISprite::UpdateCanSpriteSeePlayer(environmentMap, playerX, playerY);
    Enemy::UpdateMovement(timeStep, environmentMap, allSprites);
    DestroyableSprite::UpdateIfHit(timeStep);
    Bobbable::Bobbing();
}

// virtual method from MovableSprite that needs defining
void Enemy::UpdateMovement(float timeStep, const std::vector<int>& environmentMap, std::list<SpriteObject*>& allSprites) {
    
    if (dead) {
        if (!scoreCounted)
        {
            WhenDead();
            scoreCounted = true;
        }
        z = 0;
        return;
    }
    if (dying) {
        if (isBobbing)
            StopBobbing();

        if (z > 0)
            z -= 0.04f;

        return;
    }

    // If not dying or dead, update movement and handle collisions
    MovableSprite::UpdateVelocity(timeStep); // Update the velocity of the sprite

    // Update the hit flags
    Collidable::UpdateHitFlags(velocityX, velocityY, environmentMap, allSprites);

    // If hitting another sprite, no movement - just rotation from 'AI'
    if (hitOther) {
        return;
    }
    else { // If hitting a wall, add x or y movement accordingly - allows for wall 'sliding'
        if (!hitWallX) {
            x += velocityX;
        }
        if (!hitWallY) {
            y += velocityY;
        }
    }
    
}

// virtual method from AISprite that needs defining
void Enemy::UpdateAI(float timeStep) {

    if (canSeePlayer)
    {
        SetRotationSpeed(1.5f);
        SetSpeed(2.0f);

        if (relativeAngle < PI)
            RotateAntiClockwise(timeStep);
        else
            RotateClockwise(timeStep);
    }
    else {
        SetRotationSpeed(0.6f);
        SetSpeed(0.2f);
        RotateClockwise(timeStep);
    }
}

// add a callback to add to player score
void Enemy::SetOnDeathAddScoreCallback(std::function<void(int)> callback) {
    onDeathAddScoreCallback = callback;
}

void Enemy::WhenDead() {
    if (onDeathAddScoreCallback) {
        onDeathAddScoreCallback(1); // Notify the callback, adding 1 to the kill count
    }
}