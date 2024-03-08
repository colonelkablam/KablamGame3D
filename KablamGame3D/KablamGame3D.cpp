#include "KablamGame3D.h"

// constructors 
KablamGame3D::KablamGame3D(std::wstring newTitle)
	: sSaveFolderName{ L"Saves\\" }, sSaveFileName{ L"save_game" }, sSaveExtension{ L".kgs" } {

	// constructor stuff...
	sConsoleTitle = newTitle;
	eventLog.push_back(GetFormattedDateTime() + L" - Output of Error Log of last " + sConsoleTitle + L" session" + L":\n");
}

KablamGame3D::~KablamGame3D()
{
	// handle deleteing vectors of texture pointers
	for (auto texture : wallTextures)
		delete texture;
	for (auto texture : floorTextures)
		delete texture;
	for (auto texture : ceilingTextures)
		delete texture;
}

// virtual methods of KablamGraphicsEngine class (need defining)

bool KablamGame3D::OnGameCreate()
{
	// import level data
	// level data contains mapWall, mapFloorTiles, mapCeilingTiles vectors
#include "./Levels/level_1_data.txt"


	AddToLog(L"Map data added...");


	// import texture data
	wallTextures.push_back(new Texture(L"./Textures/wall_main.txr"));
	wallTextures.push_back(new Texture(L"./Textures/wall_door.txr"));


	floorTextures.push_back(new Texture(L"./Textures/floor_cheque.txr"));
	floorTextures.push_back(new Texture(L"./Textures/floor_lava.txr"));


	ceilingTextures.push_back(new Texture(L"./Textures/ceiling_main.txr"));
	ceilingTextures.push_back(new Texture(L"./Textures/ceiling_light.txr"));

	spriteTextures.push_back(new Texture(L"./Textures/test_sprite.txr"));

	AddToLog(L"Textures added...");

	SetResizeWindowLock(true);
	SetConsoleFocusPause(true);
	SetWindowPosition(100, 100);

	return true;
}

bool KablamGame3D::OnGameUpdate(float fElapsedTime)
{
	HandleKeyPress();

	ApplyMovementAndActions(fElapsedTime);

	// iterating through screen columns
	for (int x{ 0 }; x < GetConsoleWidth(); x++)
	{
		// calculate the ray angle for each iteration
		float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((fFOV / (float)GetConsoleWidth())) * (float)x;
		// handling ray angle wrap-around 
		if (fRayAngle > 2 * PI) fRayAngle -= 2 * PI;
		if (fRayAngle < 0) fRayAngle += 2 * PI;

		// initiate vars and flags
		float fDistanceToWall{ 1000.0f };
		float fTileHit{ -1.0f };
		float fYDistanceToWall{ 1000.0f };
		float fYTileHit = { -1.0f };
		float fXDistanceToWall{ 1000.0f };
		float fXTileHit{ -1.0f };

		// wall hit info to store
		bool bHitWall{ false };
		int nWallTypeX{ 0 };
		int nWallTypeY{ 0 };
		int nWallType{ 0 };

		// floats to hold ray x/y and then x/y offsets
		float fRayY{ 0.0f }, fRayX{ 0.0f }, fRayYO{ 0.0f }, fRayXO{ 0.0f };
		// int to store depth searched
		int nDepth{ nMaxDepthOfView };


		float fTanValue{ tanf(fRayAngle) };

		// looking at HORIZONTAL Y line collision

		// Skip if looking left (PI) or right (0) as will never intersect a horizontal line
		if (fRayAngle != 0.0f && fRayAngle != PI)
		{
			// depth of field set to zero
			nDepth = 0;
			bHitWall = false;

			if (fRayAngle < PI) // looking down
			{
				fRayY = (int)fPlayerY + 1.0f;								// next y intersect moving down
				fRayX = fPlayerX + (fRayY - fPlayerY) / fTanValue;	// x position at next y intersect
				fRayYO = 1.0f;													// y offset down (1x1 grid)
				fRayXO = fRayYO / fTanValue;							// x offest for moving 1 y
			}
			else if (fRayAngle > PI) // looking up
			{
				fRayY = (int)fPlayerY - 0.0001f;							// next y intersect moving up
				fRayX = fPlayerX + (fRayY - fPlayerY) / fTanValue;	// x position at next y intersect
				fRayYO = -1.0f;												// y offset up (1x1 grid)
				fRayXO = fRayYO / fTanValue;							// x offest for moving 1 y
			}

			// calculate HORIZONTAL 
			while (nDepth < nMaxDepthOfView)
			{
				// integer values for vector used to test if hitting wall

				int nTestX{ 0 };
				if (fRayX < 0) // as (int)-0.XX will go to 0, not -1
					nTestX = (int)fRayX - 1;
				else
					nTestX = (int)fRayX;

				int nTestY = (int)fRayY;

				// test if ray has hit a wall
				if (withinMapBounds(nTestX, nTestY))
				{
					if (getMapValue(nTestX, nTestY, mapWalls) != 0)
					{
						bHitWall = true;
						nWallTypeY = getMapValue(nTestX, nTestY, mapWalls);
						nDepth = nMaxDepthOfView;

						// find distance of wall 'hit' to 'left' side of tile
						if (fRayYO > 0) // ray looking down
							fYTileHit = 1 - (fRayX - nTestX);
						else
							fYTileHit = fRayX - nTestX;
					}
					else // add calculated offsets
					{
						fRayX += fRayXO;
						fRayY += fRayYO;
						nDepth++;
					}
				}
				else
				{
					nDepth = nMaxDepthOfView;
				}
			}

			// store distance to next HORIZONTAL line that is a wall
			if (bHitWall)
			{
				fYDistanceToWall = rayLength(fPlayerX, fPlayerY, fRayX, fRayY);
			}

		} // end of HORIZONTAL line checking


		// looking at VERTICAL X line collision

		// Skip if looking up (3*PI/2) or down PI/2 as will never intersect a vertical line
		if (fRayAngle != P2 && fRayAngle != P3)
		{
			// depth of field set to zero
			nDepth = 0;
			bHitWall = false;

			if (fRayAngle > P3 || fRayAngle < P2) // looking right
			{
				fRayX = ((int)(fPlayerX)+1.0f);								// next x intersect moving right
				fRayY = fPlayerY + (fRayX - fPlayerX) * fTanValue;	// y position at next x intersect
				fRayXO = 1;													// x offset down (1x1 grid)
				fRayYO = fRayXO * fTanValue;							// y offest for moving 1 x
			}
			else if (fRayAngle > P2 && fRayAngle < P3) // looking left
			{
				fRayX = (int)fPlayerX - 0.0001f;								// next x intersect moving left
				fRayY = fPlayerY + (fRayX - fPlayerX) * fTanValue;	// y position at next x intersect
				fRayXO = -1;												// x offset up (1x1 grid)
				fRayYO = fRayXO * fTanValue;							// y offest for moving 1 x
			}

			while (nDepth < nMaxDepthOfView)
			{
				// integer values for vector used to test if hitting wall
				int nTestY{ 0 };
				if (fRayY < 0) // as (int)-0.XX will go to 0, not -1 
					nTestY = (int)fRayY - 1;
				else
					nTestY = (int)fRayY;

				int nTestX = (int)fRayX;


				// test if ray has hit a wall
				if (withinMapBounds(nTestX, nTestY))
				{
					if (getMapValue(nTestX, nTestY, mapWalls) != 0)
					{
						bHitWall = true;
						nWallTypeX = getMapValue(nTestX, nTestY, mapWalls);
						nDepth = nMaxDepthOfView;

						// find distance of wall 'hit' to 'left' side of tile
						if (fRayXO < 0) // ray looking left
							fXTileHit = 1 - (fRayY - nTestY);
						else
							fXTileHit = fRayY - nTestY;
					}
					else // add calculated offsets
					{
						fRayX += fRayXO;
						fRayY += fRayYO;
						nDepth++;
					}
				}
				else
				{
					nDepth = nMaxDepthOfView;
				}
			}

			// store distance to next HORIZONTAL line that is a wall
			if (bHitWall)
			{
				fXDistanceToWall = rayLength(fPlayerX, fPlayerY, fRayX, fRayY);
			}

		} // end of VERTICAL line checking

		// get shortist distance and correlating tile hit axis and wall type
		if (fYDistanceToWall <= fXDistanceToWall)
		{
			fDistanceToWall = fYDistanceToWall;
			fTileHit = fYTileHit;
			nWallType = nWallTypeY;
		}
		else
		{
			fDistanceToWall = fXDistanceToWall;
			fTileHit = fXTileHit;
			nWallType = nWallTypeX;

		}

		// correct for fish-eye effect
		fDistanceToWall *= cosf(fRayAngle - fPlayerA);


		// get ratios of wall to ceiling and floor

		// height of wall calculated as a ratio of ScreenHeight() / distance, * fWallUnit means height of top of wall
		float fWall{ (GetConsoleHeight() / fDistanceToWall) * fWallHUnit };

		// calculate ceiling start according to wall height AND player height
		float fCeiling{ GetConsoleHeight() / 2 - fWall * (fWallHUnit - fPlayerH) - fPlayerTilt };
		// + (fPlayerH - fPlayerHDefault) / 3)  - will look down while jumping

		// simple to calculate floor position 
		float fFloor{ fCeiling + fWall * fWallHUnit };

		// apply ratios to draw 'heights' for each column
		int nCeiling{ (int)(fCeiling) };
		int nFloor{ (int)(fFloor) };

		// char to draw 'shade'
		short nWallShadeGlyph{ 0 };

		if (fDistanceToWall > 18)
			nWallShadeGlyph = PIXEL_QUARTER;
		else if (fDistanceToWall > 10)
			nWallShadeGlyph = PIXEL_HALF;
		else if (fDistanceToWall > 6)
			nWallShadeGlyph = PIXEL_THREEQUARTERS;
		else
			nWallShadeGlyph = PIXEL_SOLID;


		// draw the full column
		for (int y{ 0 }; y < GetConsoleHeight(); y++)
		{
			// draw ceiling
			if (y <= nCeiling)
			{
				//calculate difference from mid screen
				float dy = y - GetConsoleHeight() / 2.0f + fPlayerTilt;

				// handle dy == 0;
				if (dy == 0)
					dy = 1;

				// 'fisheye' correction
				float fRayFix = cosf(fRayAngle - fPlayerA);

				// calculate ceiling textile 'hit' x & y - looking up so * fPlayerH - fWallHUnit
				float fTileHitX = fPlayerX + cosf(fRayAngle) * (GetConsoleHeight() * fWallHUnit * (fPlayerH - fWallHUnit)) / dy / fRayFix;
				float fTileHitY = fPlayerY + sinf(fRayAngle) * (GetConsoleHeight() * fWallHUnit * (fPlayerH - fWallHUnit)) / dy / fRayFix;

				// get index for texture ceiling map
				int nTileIndexX;
				int nTileIndexY;

				// as (int)-0.XX will go to 0, not -1 
				if (fTileHitX < 0)
					nTileIndexX = (int)fTileHitX - 1;
				else
					nTileIndexX = (int)fTileHitX;

				if (fTileHitY < 0)
					nTileIndexY = (int)fTileHitY - 1;
				else
					nTileIndexY = (int)fTileHitY;

				// char to draw 'shade'
				short nCeilingShadeGlyph{ L' '};
				int nDetailLevel{ 0 };

				if (y < GetConsoleHeight() / 6.0f - (int)fPlayerTilt)
				{
					nCeilingShadeGlyph = PIXEL_SOLID;
					nDetailLevel = 0;
				}
				else if (y < GetConsoleHeight() / 4.0f - (int)fPlayerTilt)
				{
					nCeilingShadeGlyph = PIXEL_THREEQUARTERS;
					nDetailLevel = 1;
				}
				else if (y < GetConsoleHeight() / 3.0f - (int)fPlayerTilt)
				{
					nCeilingShadeGlyph = PIXEL_HALF;
					nDetailLevel = 2;
				}
				else if (y < GetConsoleHeight() / 2.0f - (int)fPlayerTilt)
				{
					nCeilingShadeGlyph = PIXEL_QUARTER;
					nDetailLevel = 3;
				}

				int nCeilingType = getMapValue(nTileIndexX, nTileIndexY, mapCeilingTiles);

				// draw corresponding pixel per ceiling tile
				if (nCeilingType == 0)
				{
					DrawPoint(x, y, FG_DARK_BLUE, PIXEL_THREEQUARTERS);
				}
				else
				{
					short colour = ceilingTextures[nCeilingType - 1]->SampleColourWithMipmap(fTileHitX - nTileIndexX, fTileHitY - nTileIndexY, nDetailLevel);

					if (ceilingTextures.at(nCeilingType - 1)->IsIlluminated())
						nCeilingShadeGlyph = PIXEL_SOLID;

					DrawPoint(x, y, colour, nCeilingShadeGlyph);
				}

			}
			// draw a wall character
			else if (y > nCeiling && y <= nFloor && nWallType != 0)
			{
				//// calculate Y sample of texture tile
				//float fSampleY = ((float)y - (float)nCeiling) / ((float)nFloor - (float)nCeiling);

				//// nShade added for depth
				//DrawPoint(x, y, wallTextures[nWallType - 1]->SampleColour(fTileHit, fSampleY), PIXEL_SOLID);

				// calculate Y sample of texture tile
				float fSampleY = ((float)y - (float)nCeiling) / ((float)nFloor - (float)nCeiling);

				if (fDistanceToWall < 0)
				{
					DrawPoint(x, y, wallTextures[nWallType - 1]->SampleColour(fTileHit, fSampleY), PIXEL_SOLID);
				}
				else {
					//CHAR_INFO pixel;
					//wallTextures[nWallType - 1]->LinearInterpolationWithGlyphShading(fTileHit, fSampleY, pixel);
					//DrawPoint(x, y, pixel.Attributes, pixel.Char.UnicodeChar);

					short colour = wallTextures[nWallType - 1]->SampleColourWithMipmap(fTileHit, fSampleY, fDistanceToWall/20);

					DrawPoint(x, y, colour, PIXEL_SOLID);
				}
			}
			// draw a floor character
			else if (y >= nFloor && y <= GetConsoleHeight())
			{
				//calculate difference from mid screen
				float dy = y - GetConsoleHeight() / 2 + fPlayerTilt;

				// handle dy == 0;
				if (dy == 0)
					dy = 1;

				// 'fisheye' correction
				float fRayFix = cosf(fRayAngle - fPlayerA);


				// calculate floor tile 'hit' x & y - looking down so * fPlayerH
				float fTileHitX = fPlayerX + cosf(fRayAngle) * (GetConsoleHeight() * fWallHUnit * fPlayerH) / dy / fRayFix;
				float fTileHitY = fPlayerY + sinf(fRayAngle) * (GetConsoleHeight() * fWallHUnit * fPlayerH) / dy / fRayFix;

				// get index for texture floor map
				int nTileIndexX;
				int nTileIndexY;
				// as (int)-0.XX will go to 0, not -1 
				if (fTileHitX < 0)
					nTileIndexX = (int)fTileHitX - 1;
				else
					nTileIndexX = (int)fTileHitX;

				if (fTileHitY < 0)
					nTileIndexY = (int)fTileHitY - 1;
				else
					nTileIndexY = (int)fTileHitY;

				// char to draw 'shade'
				short nFloorShadeGlyph{ PIXEL_SOLID };
				int nDetailLevel{ 0 };

				if (y < GetConsoleHeight() - GetConsoleHeight() / 2.5f - (int)fPlayerTilt) 
				{
					nFloorShadeGlyph = PIXEL_QUARTER;
					nDetailLevel = 4;
				}
				else if (y < GetConsoleHeight() - GetConsoleHeight() / 3.0f - (int)fPlayerTilt)
				{
					nFloorShadeGlyph = PIXEL_HALF;
					nDetailLevel = 2;

				}
				else if (y < GetConsoleHeight() - GetConsoleHeight() / 4.0f - (int)fPlayerTilt)
				{
					nFloorShadeGlyph = PIXEL_THREEQUARTERS;
					nDetailLevel = 1;
				}


				int nFloorType = getMapValue(nTileIndexX, nTileIndexY, mapFloorTiles);
				short colour;

				// draw corresponding pixel per floor tile
				if (nFloorType == 0)
				{
					DrawPoint(x, y, FG_DARK_GREEN, PIXEL_THREEQUARTERS);
				}
				else
				{
					/*colour = floorTextures[nFloorType - 1]->SampleColour(fTileHitX - nTileIndexX, fTileHitY - nTileIndexY);

					if (floorTextures.at(nFloorType - 1)->IsIlluminated() == true)
					{
						colour = colour | FOREGROUND_INTENSITY;
					}

					DrawPoint(x, y, colour, PIXEL_SOLID);*/

					////CHAR_INFO pixel;
					////floorTextures[nFloorType - 1]->LinearInterpolationWithGlyphShading(fTileHitX - nTileIndexX, fTileHitY - nTileIndexY, pixel);
					////DrawPoint(x, y, pixel.Attributes, pixel.Char.UnicodeChar);
					////CHAR_INFO pixel;
					

					colour = floorTextures[nFloorType - 1]->SampleColourWithMipmap(fTileHitX - nTileIndexX, fTileHitY - nTileIndexY, nDetailLevel);

					if (floorTextures.at(nFloorType - 1)->IsIlluminated() == true)
					{
						colour = colour | FG_INTENSITY;
					}

					DrawPoint(x, y, colour, PIXEL_SOLID);

				}




			}
		}
	} // end of screen column iteration

	//ApplyBilinearFilterScreen();

	DisplayAim();
	
	// display map and player
	if (nMapDisplayStatus != 0)
	{
		for (int x{ 0 }; x < nMapWidth; x++)
			for (int y{ 0 }; y < nMapHeight; y++)
			{
				short wcMapPixelColour = FG_BLACK;
				if (mapWalls[y * nMapWidth + x] == 1)
					wcMapPixelColour = FG_YELLOW;

				// draw an enlaged map
				if (nMapDisplayStatus > 0)
				{
					// draw player position
					int pX{ (int)fPlayerX };
					int pY{ (int)fPlayerY };
					DrawLine((pX + pX) + 1, (pY + pY) + 1, (pX + pX) + 1 + cosf(fPlayerA) * 4, (pY + pY) + 1 + sinf(fPlayerA) * 4);
					DrawPoint((pX + pX) + 1, (pY + pY) + 1, FG_DARK_RED, PIXEL_SOLID);

					// draw top partial map
					DrawPoint((x + x) + 1, (y + y) + 1, wcMapPixelColour, PIXEL_SOLID);

				}
				if (nMapDisplayStatus > 1)
				{
					// draw full map
					DrawPoint((x + x + 1) + 1, (y + y + 1) + 1, wcMapPixelColour, PIXEL_SOLID);
					DrawPoint((x + x) + 1, (y + y + 1) + 1, wcMapPixelColour, PIXEL_SOLID);
					DrawPoint((x + x + 1) + 1, (y + y) + 1, wcMapPixelColour, PIXEL_SOLID);
				}
			}
	}

	return true;
} // end of OnGameUpdate


// member methods of derived KablamGame class

// key press actions

void KablamGame3D::HandleKeyPress()
{
	// Set movement flags
	actionStates.left = GetKeyState(L'A').bHeld;
	actionStates.right = GetKeyState(L'D').bHeld;
	actionStates.forward = GetKeyState(L'W').bHeld;
	actionStates.backward = GetKeyState(L'S').bHeld;
	actionStates.rotateRight = GetKeyState(VK_RIGHT).bHeld;
	actionStates.rotateLeft = GetKeyState(VK_LEFT).bHeld;
	actionStates.lookUp = GetKeyState(VK_UP).bHeld;
	actionStates.lookDown = GetKeyState(VK_DOWN).bHeld;


	// Set action flags
	actionStates.use = GetKeyState(L'E').bPressed;
	actionStates.jump = GetKeyState(VK_SPACE).bHeld;
	actionStates.pause = GetKeyState(L'P').bPressed;
	actionStates.toggleMap = GetKeyState(L'M').bPressed;

}

bool KablamGame3D::ApplyMovementAndActions(float fElapsedTime)
{
	//Handle Rotation
	if (actionStates.rotateLeft)
	{
		fPlayerA -= (fPlayerRotationSpeed)*fElapsedTime;
		//handling player angle wrap-around
		if (fPlayerA < 0) fPlayerA += 2 * PI;
	}

	if (actionStates.rotateRight)
	{
		fPlayerA += (fPlayerRotationSpeed)*fElapsedTime;
		//handling player angle wrap-around
		if (fPlayerA > 2 * PI) fPlayerA -= 2 * PI;
	}

	// Handle looking 
	if (actionStates.lookUp)
	{
		if (fPlayerTilt > - nTiltMax)
			fPlayerTilt -= (fPlayerTiltSpeed)*fElapsedTime;
	}

	if (actionStates.lookDown)
	{
		if (fPlayerTilt <  nTiltMax)
			fPlayerTilt += (fPlayerTiltSpeed)*fElapsedTime;
	}

	// Handle Movement
	if (actionStates.forward)
	{
		fPlayerX += cosf(fPlayerA) * fPlayerSpeed * fElapsedTime;
		fPlayerY += sinf(fPlayerA) * fPlayerSpeed * fElapsedTime;

		// collision detection - reversal of movement if hitting wall
		if (!(getMapValue((int)fPlayerX, (int)fPlayerY, mapWalls) == 0))
		{
			fPlayerX -= cosf(fPlayerA) * fPlayerSpeed * fElapsedTime;
			fPlayerY -= sinf(fPlayerA) * fPlayerSpeed * fElapsedTime;
		}
	}

	if (actionStates.backward)
	{
		fPlayerX -= cosf(fPlayerA) * fPlayerSpeed * fElapsedTime;
		fPlayerY -= sinf(fPlayerA) * fPlayerSpeed * fElapsedTime;

		// collision detection - reversal of movement if hitting wall
		if (!(getMapValue((int)fPlayerX, (int)fPlayerY, mapWalls) == 0))
		{
			fPlayerX += cosf(fPlayerA) * fPlayerSpeed * fElapsedTime;
			fPlayerY += sinf(fPlayerA) * fPlayerSpeed * fElapsedTime;
		}
	}

	if (actionStates.right)
	{
		fPlayerX += cosf(fPlayerA + PI / 2) * fPlayerSpeed * fElapsedTime;
		fPlayerY += sinf(fPlayerA + PI / 2) * fPlayerSpeed * fElapsedTime;

		// collision detection - reversal of movement if hitting wall
		if (!(getMapValue((int)fPlayerX, (int)fPlayerY, mapWalls) == 0))
		{
			fPlayerX -= cosf(fPlayerA + PI / 2) * fPlayerSpeed * fElapsedTime;
			fPlayerY -= sinf(fPlayerA + PI / 2) * fPlayerSpeed * fElapsedTime;
		}
	}

	if (actionStates.left)
	{
		fPlayerX += cosf(fPlayerA - PI / 2) * fPlayerSpeed * fElapsedTime;
		fPlayerY += sinf(fPlayerA - PI / 2) * fPlayerSpeed * fElapsedTime;

		// collision detection - reversal of movement if hitting wall
		if (!(getMapValue((int)fPlayerX, (int)fPlayerY, mapWalls) == 0))
		{
			fPlayerX -= cosf(fPlayerA - PI / 2) * fPlayerSpeed * fElapsedTime;
			fPlayerY -= sinf(fPlayerA - PI / 2) * fPlayerSpeed * fElapsedTime;
		}
	}

	// handle ACTION keys
	if (actionStates.jump && !bPlayerJumping)
	{
		fPlayerUpVelocity = fPlayerJumpPower;
		bPlayerJumping = true;
	}

	// jump mechanics
	if (bPlayerJumping)
	{
		fPlayerUpVelocity += fGravity * fElapsedTime;
		fPlayerH += fPlayerUpVelocity * fElapsedTime;

		if (fPlayerH <= fPlayerHDefault)
		{
			bPlayerJumping = false;
			fPlayerH = fPlayerHDefault;
			fPlayerUpVelocity = 0.0f;
		}
	}

	// use 

	if (actionStates.use)
	{
		DisplayAlertMessage(L"Use button pressed.");
	}

	// map
	if (actionStates.toggleMap)
		nMapDisplayStatus = (nMapDisplayStatus + 1) % 3;
	
	// pause
	if (actionStates.pause)
		DisplayAlertMessage(L"Game Paused.");

	return true;
}


// get ray length from opposite and adjacent sides of ray vector
float KablamGame3D::rayLength(float px, float py, float rx, float ry) const
{
	// using a2 = sqrt(b2 + c2)
	float b = rx - px;
	float c = ry - py;

	return sqrt(b * b + c * c);
}

// check if within map bounds
bool KablamGame3D::withinMapBounds(int x, int y) const
{
	return (x >= 0 && x < nMapWidth) && (y >= 0 && y < nMapHeight);
}

// get map value at coord x,y
int KablamGame3D::getMapValue(int x, int y, const std::vector<int>& map) const
{
	if (withinMapBounds(x, y))
	{
		return map[y * nMapWidth + x];
	}
	else
		return 0;
}

// display aiming cross
void KablamGame3D::DisplayAim(short colour, short glyph)
{
	int xOff = int(GetConsoleWidth() / 2) - 2;
	int yOff = int(GetConsoleHeight() / 2) - 2;

	for (size_t x{ 0 }; x < 5; x++)
		for (size_t y{ 0 }; y < 5; y++)
		{
			if (aimArray[y * 5 + x] == 1)
				DrawPoint(x + xOff, y + yOff, colour, glyph);
		}
}
