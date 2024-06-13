#pragma once

#include <Windows.h>
#include <vector>
#include "Texture.h"
#include "SoundManager.h"

class Door : public Texture
{
private:
    int xPos;
    int yPos;
    bool opening;
    bool closing;
    bool open; // True if fully open, false if fully closed
    float amountOpen;
    bool needsKey;
    int keyType;
    float timeStayOpen;
    float openTimer;
    Texture* doorTexture;
    SoundManager* soundManager;

public:
    Door(int x, int y, Texture* texture, SoundManager* soundManager = nullptr);

    ~Door();

    void StartOpen();

    void PlayOpen();

    void StartClose();

    void PlayClose();

    float GetAmountOpen() const;

    bool IsOpen() const;

    bool IsClosed() const;

    void UnlockDoor();

    void LockDoor();

    void UpdateDoor(float timeStep, std::vector<int>& enviroMap, int playerX, int playerY);

    CHAR_INFO SamplePixel(float x, float y) const override;
};

