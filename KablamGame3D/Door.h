#pragma once

#include <Windows.h>
#include "Texture.h"
class Door
{
private:
	bool open;
	float amountOpen;
	bool needsKey;
	int keyType;
	float timeStayOpen;
	Texture* doorTexture;

public:
	Door(Texture* texture);

	~Door();

	void OpenDoor();

	float GetAmountOpen();


	void CloseDoor();

	void UnlockDoor();

	void LockDoor();

	CHAR_INFO GetPixel(float x, float y);

};

