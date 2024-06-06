#pragma once

#include "Enemy.h"
#include "Projectile.h"
#include "FloorLampSprite.h"

class SpriteFactory {
public:
    virtual ~SpriteFactory() = default;
    virtual SpriteObject* CreateSprite(float x, float y, float z, float initAngle = 0.0f) const = 0;
};

class OctoFactory : public SpriteFactory {
private:
    Texture* aliveTexture;
    Texture* hitTexture;
    Texture* dyingTexture;
    Texture* deadTexture;
    SoundManager* soundManager;

public:
    OctoFactory(Texture* alive, Texture* hit, Texture* dying, Texture* dead, float z, SoundManager* sounds)
        : aliveTexture(alive), hitTexture(hit), dyingTexture(dying), deadTexture(dead), soundManager(sounds) {}

    SpriteObject* CreateSprite(float x, float y, float z, float initAngle) const override {

        return new Enemy{ x, y, z, aliveTexture, hitTexture, dyingTexture, deadTexture, 2, 2, false, soundManager };
    }
};


class BulletFactory : public SpriteFactory {
private:
    Texture* aliveTexture;
    Texture* dyingTexture;
    SoundManager* soundManager;


public:
    BulletFactory(Texture* alive, Texture* dying, float z, SoundManager* sounds)
        : aliveTexture(alive), dyingTexture(dying), soundManager(sounds) {}

    SpriteObject* CreateSprite(float x, float y, float z, float angle) const override {
        return new Projectile{ x, y, z, aliveTexture, dyingTexture, angle, soundManager };
    }
};

class FloorlampFactory : public SpriteFactory {
private:
    Texture* aliveTexture;

public:
    FloorlampFactory(Texture* alive)
        : aliveTexture(alive) {}

    SpriteObject* CreateSprite(float x, float y, float z, float angle) const override {
        return new FloorLampSprite{ x, y, z, aliveTexture };
    }
};
