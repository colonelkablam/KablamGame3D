#pragma once

#include "KablamGame3D.h"

#include "SpriteObject.h"
#include "SpecificSpriteClasses.h"

class SpriteFactory {
public:
    virtual ~SpriteFactory() = default;
    virtual SpriteObject* CreateSprite(float x, float y, float z, float initAngle = 0.0f) const = 0;
};

class OctoFactory : public SpriteFactory {
private:
    Texture* aliveTexture;
    Texture* deadTexture;
    Texture* hitTexture;

public:
    OctoFactory(Texture* alive, Texture* dead, Texture* hit)
        : aliveTexture(alive), deadTexture(dead), hitTexture(hit) {}

    SpriteObject* CreateSprite(float x, float y, float z, float initAngle) const override {

        return new OctoSprite{ x, y, z, aliveTexture, deadTexture, hitTexture, 2, 2, false };
    }
};


class BulletFactory : public SpriteFactory {
private:
    Texture* aliveTexture;
    Texture* hitTexture;

public:
    BulletFactory(Texture* alive, Texture* hit)
        : aliveTexture(alive), hitTexture(hit) {}

    SpriteObject* CreateSprite(float x, float y, float z, float angle) const override {
        return new BulletSprite{ x, y, z, aliveTexture, hitTexture, angle };
    }
};

class FloorlampFactory : public SpriteFactory {
private:
    Texture* aliveTexture;

public:
    FloorlampFactory(Texture* alive)
        : aliveTexture(alive) {}

    SpriteObject* CreateSprite(float x, float y, float z, float angle) const override {
        return new FloorlampSprite{ x, y, z, aliveTexture };
    }
};
