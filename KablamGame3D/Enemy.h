#pragma once
#include "IntermediateSpriteClasses.h"

class Enemy : public Collidable, public MovableSprite, public RotatableSprite, public DestroyableSprite, public AISprite, public Bobbable {

protected:

public:
    Enemy(float initX, float initY, float initZ, Texture* initAliveSprite, Texture* initDeadSprite, Texture* initHitSprite, int initAggression, int initFireRate, bool initIsDead = false);

    virtual ~Enemy() = default;

    void UpdateSprite(float timeStep, float playerX, float playerY, float playerTilt, const std::vector<int>& floorMap, std::list<SpriteObject*>& allSprites) override;

    void UpdateMovement(float timeStep, const std::vector<int>& floorMap, std::list<SpriteObject*>& allSprites) override;

    void UpdateAI(float timeStep) override;
};

