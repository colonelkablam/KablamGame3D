#include "Door.h"

#include "Door.h"

Door::Door(Texture* texture)
    : doorTexture(texture), opening(false), closing(false), open(false), amountOpen(0.0f), needsKey(false), keyType(0), timeStayOpen(3.0f), openTimer(0.0f)
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
}

void Door::StartClose()
{
    closing = true;
    opening = false;
}

float Door::GetAmountOpen() const
{
    return amountOpen;
}

bool Door::IsOpen() const
{
    return open && amountOpen == 1.0f;
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

void Door::UpdateDoor(float timeStep)
{
    if (opening)
    {
        amountOpen += timeStep;
        if (amountOpen >= 1.0f)
        {
            amountOpen = 1.0f;
            opening = false;
            open = true;
            openTimer = timeStayOpen;
        }
    }
    else if (closing)
    {
        amountOpen -= timeStep;
        if (amountOpen <= 0.0f)
        {
            amountOpen = 0.0f;
            closing = false;
            open = false;
        }
    }

    // If the door is fully open, count down the open timer
    if (open && amountOpen == 1.0f && openTimer > 0.0f)
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
