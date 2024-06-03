#pragma once
#include "IntermediateSpriteClasses.h"

class Projectile : public Collidable, public MovableSprite, public DestroyableSprite {

public:
    Projectile(float initX, float initY, float initZ, Texture* initAliveSprite, Texture* initDyingSprite, float initAngle);

    virtual ~Projectile() = default;

    void UpdateSprite(float timeStep, float playerX, float playerY, float playerTilt, const std::vector<int>& floorMap, std::list<SpriteObject*>& allSprites) override;

private:
    void UpdateMovement(float timeStep, const std::vector<int>& floorMap, std::list<SpriteObject*>& allSprites) override;
    bool CheckCollisions(std::list<SpriteObject*>& allSprites);
    bool IsCollidingWith(Collidable* other);
    bool RayIntersectsCircle(float cx, float cy, float radius);
    void HandleCollision(DestroyableSprite* target);
};

