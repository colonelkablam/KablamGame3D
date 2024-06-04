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

public:
    OctoFactory(Texture* alive, Texture* hit, Texture* dying, Texture* dead, float z)
        : aliveTexture(alive), hitTexture(hit), dyingTexture(dying), deadTexture(dead) {}

    SpriteObject* CreateSprite(float x, float y, float z, float initAngle) const override {

        return new Enemy{ x, y, z, aliveTexture, hitTexture, dyingTexture, deadTexture, 2, 2, false };
    }
};


class BulletFactory : public SpriteFactory {
private:
    Texture* aliveTexture;
    Texture* dyingTexture;

public:
    BulletFactory(Texture* alive, Texture* dying, float z)
        : aliveTexture(alive), dyingTexture(dying) {}

    SpriteObject* CreateSprite(float x, float y, float z, float angle) const override {
        return new Projectile{ x, y, z, aliveTexture, dyingTexture, angle };
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
