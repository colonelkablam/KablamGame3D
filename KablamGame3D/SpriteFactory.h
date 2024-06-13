#pragma once

#include "Enemy.h"
#include "Player.h"
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
    Player* player;

public:
    OctoFactory(Texture* alive, Texture* hit, Texture* dying, Texture* dead, float z, SoundManager* sounds, Player* p)
        : aliveTexture(alive), hitTexture(hit), dyingTexture(dying), deadTexture(dead), soundManager(sounds), player{ p } {}

    SpriteObject* CreateSprite(float x, float y, float z, float initAngle) const override {
        Enemy* octobaddy = new Enemy{ x, y, z, aliveTexture, hitTexture, dyingTexture, deadTexture, 2, 2, false, soundManager };
        octobaddy->SetOnDeathAddScoreCallback([this](int points) {
            player->IncreaseScore(points);
            });
        return octobaddy;
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
