#include "Door.h"

Door::Door(Texture* texture)
	: doorTexture(texture), open(false), needsKey (false), keyType(0), timeStayOpen(3.0f) {}

Door::~Door()
{
}

void Door::OpenDoor()
{
}

void Door::CloseDoor()
{
}

void Door::UnlockDoor()
{
}

void Door::LockDoor()
{
}

CHAR_INFO Door::GetPixel(float x, float y)
{
    return doorTexture->SamplePixel(x, y);
}
