#include "Projectile.h"
#include "SoundManager.h"

Projectile::Projectile(float initX, float initY, float initZ, Texture* initAliveSprite, Texture* initDyingSprite, float initAngle, SoundManager* sounds)
    : SpriteObject(initX, initY, initZ, SpriteType::BULLET_TYPE, true, SPRITE_TEXTURE_WIDTH, SPRITE_TEXTURE_HEIGHT, initAliveSprite),
    MovableSprite(16.0f, 16.0f, initAngle, 0.0f),
    Collidable(0.0f),
    DestroyableSprite(1.0f, initAliveSprite, initAliveSprite, initDyingSprite, 0.0f, 0.0f, 0.2f, false),
    MakesNoise (sounds)
{

    soundManager->PlaySoundByName(L"shootFireball");
}

void Projectile::UpdateSprite(float timeStep, float playerX, float playerY, float playerTilt, const std::vector<int>& environmentMap, std::list<SpriteObject*>& allSprites) {
    // update position relative to player
    SpriteObject::UpdateTimeAndDistanceToPlayer(timeStep, playerX, playerY);

    // see if dying
    if (DestroyableSprite::IsSpriteDead()) {
        DestroyableSprite::UpdateIfHit(timeStep); // update dying texture and time
    }
    else {
        Projectile::UpdateMovement(timeStep, environmentMap, allSprites);
    }
}

void Projectile::UpdateMovement(float timeStep, const std::vector<int>& environmentMap, std::list<SpriteObject*>& allSprites) {
    MovableSprite::UpdateVelocity(timeStep); // update the velocity per frame of sprite

    // update the hit flags
    Collidable::UpdateHitFlags(velocityX, velocityY, environmentMap, allSprites);

    // just need if newX and newY walltile is a hit as no 'sliding' for projectiles.
    if (hitWallXY) {
        soundManager->PlaySoundByName(L"fireballHit", false, 1 - distToPlayer/32);
        DestroyableSprite::MakeDead();
    }
    else if (!CheckCollisions(allSprites)) {
        x += velocityX;
        y += velocityY;
    }
}

bool Projectile::CheckCollisions(const std::list<SpriteObject*>& allSprites) {
    for (auto* other : allSprites) {
        if (other == this) continue; // Skip self

        DestroyableSprite* destroyableTarget = dynamic_cast<DestroyableSprite*>(other);
        Collidable* collidableTarget = dynamic_cast<Collidable*>(other);
        Projectile* projectileTarget = dynamic_cast<Projectile*>(other);

        // Skip collision check if the other sprite is also a Projectile
        if (projectileTarget) continue;

        if (destroyableTarget && collidableTarget && IsCollidingWith(collidableTarget) && !destroyableTarget->IsSpriteDead() && !destroyableTarget->IsSpriteDying()) {
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
    // Handle the collision (both the bullet and target are affected)
    soundManager->PlaySoundByName(L"fireballHit", false, 1 - distToPlayer / 32);
    MakeDead(); // Mark the bullet as dead
    target->MakeHit();
    target->SetDamage(35);
}