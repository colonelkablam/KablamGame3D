#pragma once
#include "IntermediateSpriteClasses.h"

class Projectile : public Collidable, public MovableSprite, public DestroyableSprite, public MakesNoise {

public:
    Projectile(float initX, float initY, float initZ, Texture* initAliveSprite, Texture* initDyingSprite, float initAngle, SoundManager* sounds = nullptr);

    virtual ~Projectile() = default;

    void UpdateSprite(float timeStep, float playerX, float playerY, float playerTilt, const std::vector<int>& floorMap, std::list<SpriteObject*>& allSprites) override;

private:
    void UpdateMovement(float timeStep, const std::vector<int>& environmentMap, std::list<SpriteObject*>& allSprites) override;
    bool CheckCollisions(const std::list<SpriteObject*>& allSprites);
    bool IsCollidingWith(Collidable* other);
    bool RayIntersectsCircle(float cx, float cy, float radius);
    void HandleCollision(DestroyableSprite* target);
};

