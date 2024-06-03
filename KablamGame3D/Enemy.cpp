#include "Enemy.h"

Enemy::Enemy(float initX, float initY, float initZ, Texture* initAliveSprite, Texture* initDeadSprite, Texture* initHitSprite, int initAggression, int initFireRate, bool initIsDead)
    : SpriteObject(initX, initY, initZ, SpriteType::OCTO_TYPE, false, SPRITE_TEXTURE_WIDTH, SPRITE_TEXTURE_HEIGHT, initAliveSprite),
    MovableSprite(1.5f, 10.0f, 0.0f, 0.03f),
    Collidable(0.3f),
    RotatableSprite(),
    DestroyableSprite(100.0f, initDeadSprite, initHitSprite, nullptr, initIsDead),
    AISprite(initAggression, initFireRate),
    Bobbable(2.0f, 1.15f) {}

void Enemy::UpdateSprite(float timeStep, float playerX, float playerY, float playerTilt, const std::vector<int>& floorMap, std::list<SpriteObject*>& allSprites) {
    SpriteObject::UpdateTimeAndDistanceToPlayer(timeStep, playerX, playerY);
    RotatableSprite::UpdateRelativeAngleToPlayer();
    Enemy::UpdateAI(timeStep);
    Enemy::UpdateMovement(timeStep, floorMap, allSprites);
    DestroyableSprite::UpdateIfHit(timeStep);
    Bobbable::Bobbing();
}

// virtual method from MovableSprite that needs defining
void Enemy::UpdateMovement(float timeStep, const std::vector<int>& floorMap, std::list<SpriteObject*>& allSprites) {
    MovableSprite::UpdateVelocity(timeStep); // update the velocity of sprite

    // update the hit flags
    Collidable::UpdateHitFlags(velocityX, velocityY, floorMap, allSprites);

    // if hitting another sprite no movement - just rotation from 'AI'
    if (hitOther) {
        return;
    }
    else { // if hitting a wall add x or y movement accordingly - allows for wall 'sliding'
        if (!hitWallX)
            x += velocityX;
        if (!hitWallY)
            y += velocityY;
    }
}

// virtual method from AISprite that needs defining
void Enemy::UpdateAI(float timeStep) {
    if (relativeAngle < PI)
        RotateAntiClockwise();
    else
        RotateClockwise();
}