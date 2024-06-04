#pragma once
#include "IntermediateSpriteClasses.h"

class Enemy : public Collidable, public MovableSprite, public RotatableAnimatable, public DestroyableSprite, public AISprite, public Bobbable, public DontCleanUpWhenDead {

protected:

public:
    Enemy(float initX, float initY, float initZ, Texture* initAliveSprite, Texture* initHitSprite = nullptr, Texture* initDyingSprite = nullptr, Texture* initDeadSprite = nullptr, int initAggression = 0, int initFireRate = 1, bool initIsDead = false);

    virtual ~Enemy() = default;

    void UpdateSprite(float timeStep, float playerX, float playerY, float playerTilt, const std::vector<int>& floorMap, std::list<SpriteObject*>& allSprites) override;

    void UpdateMovement(float timeStep, const std::vector<int>& floorMap, std::list<SpriteObject*>& allSprites) override;

    void UpdateAI(float timeStep) override;
};

