#pragma once

#include <functional>

#include "IntermediateSpriteClasses.h"

class Enemy : public Collidable, public MovableSprite, public RotatableAnimatable, public DestroyableSprite, public AISprite, public Bobbable, public DontCleanUpWhenDead, public MakesNoise {

protected:
    std::function<void(int)> onDeathAddScoreCallback;
    bool scoreCounted;

public:
    Enemy(float initX, float initY, float initZ, Texture* initAliveSprite, Texture* initHitSprite = nullptr, Texture* initDyingSprite = nullptr, Texture* initDeadSprite = nullptr, int initAggression = 0, int initFireRate = 1, bool initIsDead = false, SoundManager* sounds = nullptr);

    virtual ~Enemy() = default;

    void UpdateSprite(float timeStep, float playerX, float playerY, float playerTilt, const std::vector<int>& floorMap, std::list<SpriteObject*>& allSprites) override;

    void UpdateMovement(float timeStep, const std::vector<int>& floorMap, std::list<SpriteObject*>& allSprites) override;

    void UpdateAI(float timeStep) override;

    void SetOnDeathAddScoreCallback(std::function<void(int)> callback);

    void WhenDead();

};

