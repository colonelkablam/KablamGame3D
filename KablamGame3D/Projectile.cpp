#include "Projectile.h"

Projectile::Projectile(float initX, float initY, float initZ, Texture* initAliveSprite, Texture* initDyingSprite, float initAngle)
    : SpriteObject(initX, initY, initZ, SpriteType::BULLET_TYPE, false, SPRITE_TEXTURE_WIDTH, SPRITE_TEXTURE_HEIGHT, initAliveSprite),
    MovableSprite(16.0f, 16.0f, initAngle, 0.0f),
    Collidable(0.0f),
    DestroyableSprite(1.0f, nullptr, initDyingSprite, nullptr, false) {}

void Projectile::UpdateSprite(float timeStep, float playerX, float playerY, float playerTilt, const std::vector<int>& floorMap, std::list<SpriteObject*>& allSprites) {
    // update position relative to player
    SpriteObject::UpdateTimeAndDistanceToPlayer(timeStep, playerX, playerY);

    // see if dying
    if (DestroyableSprite::IsSpriteDying()) {
        DestroyableSprite::UpdateIfHit(timeStep); // update dying texture and time
    }
    else {
        Projectile::UpdateMovement(timeStep, floorMap, allSprites);
    }
}

void Projectile::UpdateMovement(float timeStep, const std::vector<int>& floorMap, std::list<SpriteObject*>& allSprites) {
    MovableSprite::UpdateVelocity(timeStep); // update the velocity of sprite

    // update the hit flags
    Collidable::UpdateHitFlags(velocityX, velocityY, floorMap, allSprites);

    if (hitWallX || hitWallY) {
        DestroyableSprite::MakeDying();
    }
    else if (!CheckCollisions(allSprites)) {
        x += velocityX;
        y += velocityY;
    }
}

bool Projectile::CheckCollisions(std::list<SpriteObject*>& allSprites) {
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

bool Projectile::IsCollidingWith(Collidable* other) {
    return RayIntersectsCircle(other->GetX(), other->GetY(), other->GetCollisionBuffer());
}

bool Projectile::RayIntersectsCircle(float cx, float cy, float radius) {
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

    return (t1 >= 0 && t1 <= 1) || (t2 >= 0 && t2 <= 1);
}

void Projectile::HandleCollision(DestroyableSprite* target) {
    // Handle the collision (e.g., mark both the bullet and the target as hit or dying)
    //SetHit(true); // Mark the bullet as hit
    MakeDying(); // Mark the bullet as dying
    target->SetHit(true); // Mark the target as hit
    target->SetDamage(35);
    //target->MakeDying(); // Mark the target as dying
}