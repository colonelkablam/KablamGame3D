# pragma once

#include <Windows.h>
#include <list>
#include <vector>
#include "GameConstants.h"
#include "Texture.h"
#include "SpriteEnums.h"

class SpriteObject {
protected:
    float x;
    float y;
    float z; // height off floor
    float baseZ; // starting height
    float distToPlayer;
    float angleToPlayer;
    float facingAngle;
    int width;
    int height;
    SpriteType type;
    bool illuminated;
    Texture* currentSprite;
    float timeElapsed;
    float collisionBuffer; // effective physical size in game from center-line


public:
    SpriteObject(float initX = 0.0f, float initY = 0.0f, float initZ = 0.0f, SpriteType initType = SpriteType::NO_TYPE, bool isIlluminated = false,
        int spriteWidth = 32, int spriteHeight = 32, Texture* initSprite = nullptr, float initAngle = 0.0f);

    virtual ~SpriteObject() = default;

    // UpdateSprite has to be overridden by specific class definition
    virtual void UpdateSprite(float timeStep, float playerX, float playerY, float playerTilt,
        std::list<SpriteObject*>& allSprites, const std::vector<int>& floorMap) = 0;    
    
    // effectively RENDER - GetPixel can be overridden by specific class definition depending of attributes
    virtual CHAR_INFO GetPixel(int x, int y) const;
    
    float GetDistanceFromPlayer() const;
    float GetDistanceFromOther(float otherX, float otherY) const;
    float GetAngleToPlayer() const;
    int GetSpriteWidth() const;
    int GetSpriteHeight() const;
    float GetX() const;
    float GetY() const;
    float GetZ() const;
    bool GetIlluminated() const;
    SpriteType GetSpriteType() const;
    float GetCollisionBuffer() const;

protected:
    void UpdateTimeAndDistanceToPlayer(float timeStep, float playerX, float playerY);
};
