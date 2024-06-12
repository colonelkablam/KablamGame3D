#pragma once

#include <Windows.h>
#include "Texture.h"
#include "SoundManager.h"

class Door : public Texture
{
private:
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
    Door(Texture* texture, SoundManager* soundManager = nullptr);

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

    void UpdateDoor(float timeStep);

    CHAR_INFO SamplePixel(float x, float y) const override;
};

