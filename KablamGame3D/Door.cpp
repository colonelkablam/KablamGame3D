#include "Door.h"

#include "GameConstants.h"

Door::Door(int x, int y, Texture* texture, SoundManager* sManager)
    : xPos(x), yPos(y), doorTexture(texture), soundManager(sManager), opening(false), closing(false), open(false), amountOpen(0.0f), needsKey(false), keyType(0),
        timeStayOpen(3.0f), openTimer(0.0f)
{
}

Door::~Door()
{
}

void Door::StartOpen()
{
    opening = true;
    closing = false;
    openTimer = 0.0f;
    PlayOpen();
}

void Door::StartClose()
{
    closing = true;
    opening = false;
    PlayClose();
}

void Door::PlayOpen()
{
    soundManager->PlaySoundByName(L"doorMainOpen");
}

void Door::PlayClose()
{
    soundManager->PlaySoundByName(L"doorMainClose");
}

float Door::GetAmountOpen() const
{
    return amountOpen;
}

bool Door::IsOpen() const
{
    return open && amountOpen == 0.9f;
}

bool Door::IsClosed() const
{
    return !open && amountOpen == 0.0f;
}

void Door::UnlockDoor()
{
    needsKey = false;
}

void Door::LockDoor()
{
    needsKey = true;
}

void Door::UpdateDoor(float timeStep, std::vector<int>& environmentMap, int playerX, int playerY)
{
    if (opening)
    {
        amountOpen += 2.5 * timeStep;
        if (amountOpen >= 0.9f)
        {
            amountOpen = 0.9f;
            opening = false;
            open = true;
            openTimer = timeStayOpen;
            environmentMap.at(yPos * MAP_WIDTH + xPos) = 0; // set environemnt at this block to passable
        }
    }
    else if (closing)
    {
        if ((int)playerX == xPos && (int)playerY == yPos)
        {
            StartOpen();
        }
        else {
            amountOpen -= timeStep;

            if (amountOpen <= 0.0f)
            {
                amountOpen = 0.0f;
                closing = false;
                open = false;
                environmentMap.at(yPos * MAP_WIDTH + xPos) = 1; // set environemnt at this block to passable
            }
        }

    }

    // If the door is fully open, count down the open timer
    if (open && amountOpen == 0.9f && openTimer > 0.0f)
    {
        openTimer -= timeStep;
        if (openTimer <= 0.0f)
        {
            StartClose();
        }
    }
}

CHAR_INFO Door::SamplePixel(float x, float y) const
{
    return doorTexture->SamplePixel(x - amountOpen, y);
}
