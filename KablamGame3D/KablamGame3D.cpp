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
	wallTextures.push_back(nullptr); // position 0 will return nullptr
	wallTextures.push_back(new Texture(L"./Textures/wall_main.txr"));
	wallTextures.push_back(new Texture(L"./Textures/wall_door.txr"));

	floorTextures.push_back(nullptr); // position 0 will return nullptr
	floorTextures.push_back(new Texture(L"./Textures/floor_cheque.txr"));
	floorTextures.push_back(new Texture(L"./Textures/floor_lava.txr"));

	ceilingTextures.push_back(nullptr); // position 0 will return nullptr
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

	//RenderScreen();

	// calculate ratio to multiply screen column by to get ray angle
	float fAngleIncrement = FOV / static_cast<float>(GetConsoleWidth());
	float fStartingAngle = fPlayerA - FOV / 2.0f;

	// iterating through screen columns
	for (int x{ 0 }; x < GetConsoleWidth(); x++)
	{
		// calculate the ray angle for each column iteration
		float fRayAngle = fStartingAngle + fAngleIncrement * static_cast<float>(x);
		// handling ray angle wrap-around 
		if (fRayAngle > 2 * PI) 
			fRayAngle -= 2 * PI;
		if (fRayAngle < 0)
			fRayAngle += 2 * PI;


		// decalare and initialise variables to store HORIZONTAL distances
		float fYDistanceToWall{ 1000.0f };
		float fYTileHit{ -1.0f };
		int nWallTypeY{ 0 };

		// looking at HORIZONTAL Y line collision
		SetHorizontalWallCollisionValues(fRayAngle, fYDistanceToWall, fYTileHit, nWallTypeY);


		// decalare and initialise variables to store VERTICAL distances
		float fXDistanceToWall{ 1000.0f };
		float fXTileHit{ -1.0f };
		int nWallTypeX{ 0 };

		// looking at VERTICAL X line collision
		SetVerticalWallCollisionValues(fRayAngle, fXDistanceToWall, fXTileHit, nWallTypeX);


		// decalare variables to store final results
		float fDistanceToWall{ 1000.0f };
		float fTileHit{ -1.0f };
		int nWallType{ 0 };

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
		// - how much of the screen to draw as ceiling/wall/floor

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


		// draw the full column
		for (int y{ 0 }; y < GetConsoleHeight(); y++)
		{
			// draw CEILING character
			if (y <= nCeiling)
			{
				// default sky pixel
				CHAR_INFO pixel;
				pixel.Attributes = FG_BLUE | BG_RED;
				pixel.Char.UnicodeChar = PIXEL_HALF;

				// declare storage for ceiling ray hit
				FloatCoord ceilingHitCoords{ 0.0f, 0.0f };
				COORD ceilingHitIndex{ 0, 0 };

				// calculate the ceiling hit coords/index -> pass by ref storage coords
				SetHorizontalSurfaceHitCoords(y, fRayAngle, ceilingHitCoords, ceilingHitIndex, true); // looking up so true flag

				// how far the hit position is from the player position
				float fDistanceToCeiling{ RayLength(fPlayerX, fPlayerY, ceilingHitCoords.X, ceilingHitCoords.Y) };

				// sample xy coords within texture
				float fXTextureTileHit{ ceilingHitCoords.X - ceilingHitIndex.X };
				float fYTextureTileHit{ ceilingHitCoords.Y - ceilingHitIndex.Y };

				// char to draw 'shade'
				short nCeilingShadeGlyph = GetGlyphShadeByDistance(fDistanceToCeiling);

				// detail level (if mipmapping used)
				int nDetailLevel = GetMipmapDetailLevel(fDistanceToCeiling);

				// texture to use
				int nCeilingType = GetMapValue(ceilingHitIndex.X, ceilingHitIndex.Y, mapCeilingTiles);

				// draw corresponding pixel per ceiling tile
				if (ceilingTextures[nCeilingType] == nullptr) // handle nullptr
				{
					DrawPoint(x, y, pixel);
				}
				else
				{
					short colour = ceilingTextures[nCeilingType]->SampleColour(fXTextureTileHit, fYTextureTileHit);
					if (ceilingTextures.at(nCeilingType)->IsIlluminated())
						nCeilingShadeGlyph = PIXEL_QUARTER;

					DrawPoint(x, y, colour, nCeilingShadeGlyph);
				}

			}
			// draw a WALL character
			else if (y > nCeiling && y <= nFloor && nWallType != 0)
			{
				// default wall pixel
				CHAR_INFO pixel;
				pixel.Attributes = FG_RED | BG_CYAN;
				pixel.Char.UnicodeChar = PIXEL_HALF;

				// char to draw 'shade'
				short nWallShadeGlyph = GetGlyphShadeByDistance(fDistanceToWall);
				int nMipmapDetailLevel = GetMipmapDetailLevel(fDistanceToWall);

				// calculate Y sample of texture tile (ratio of y value and wall height in pixels)
				float fSampleY = ((float)y - (float)nCeiling) / ((float)nFloor - (float)nCeiling);

				if (fDistanceToWall < 0)
				{
					DrawPoint(x, y, wallTextures[nWallType]->SampleColour(fTileHit, fSampleY), PIXEL_SOLID);
				}
				else {
					pixel = wallTextures[nWallType]->LinearInterpolationWithGlyphShading(fTileHit, fSampleY);
					//short colour = wallTextures[nWallType]->SampleColour(fTileHit, fSampleY);

					DrawPoint(x, y, pixel);
				}

				
			}
			// draw a FLOOR character
			else if (y >= nFloor && y <= GetConsoleHeight())
			{
				// default floor pixel
				CHAR_INFO pixel;
				pixel.Attributes = FG_GREEN | BG_RED;
				pixel.Char.UnicodeChar = PIXEL_HALF;

				// declare storage for floor ray hit
				FloatCoord floorHitCoords{ 0.0f, 0.0f };
				COORD floorHitIndex{ 0, 0 };

				// calculate the floor hit coords/index -> pass by ref storage coords
				SetHorizontalSurfaceHitCoords(y, fRayAngle, floorHitCoords, floorHitIndex, false); // looking down so false flag

				// how far the hit position is from the player position
				float fDistanceToFloor{ RayLength(fPlayerX, fPlayerY, floorHitCoords.X, floorHitCoords.Y) };

				// sample xy coords within texture
				float fXTextureTileHit{ floorHitCoords.X - floorHitIndex.X };
				float fYTextureTileHit{ floorHitCoords.Y - floorHitIndex.Y };

				// char to draw 'shade'
				short nFloorShadeGlyph = GetGlyphShadeByDistance(fDistanceToFloor);

				// detail level (if mipmapping used)
				int nDetailLevel = GetMipmapDetailLevel(fDistanceToFloor);

				// texture to use
				int nFloorType = GetMapValue(floorHitIndex.X, floorHitIndex.Y, mapFloorTiles);

				// draw corresponding pixel per ceiling tile
				if (floorTextures[nFloorType] == nullptr) // handle nullptr
				{
					DrawPoint(x, y, pixel);
				}
				else
				{
					short colour = floorTextures[nFloorType]->SampleColour(fXTextureTileHit, fYTextureTileHit);
					if (floorTextures.at(nFloorType)->IsIlluminated())
						nFloorShadeGlyph = PIXEL_SOLID;

					DrawPoint(x, y, colour, nFloorShadeGlyph);
				}
			}

		} // end of column
	} // end of screen column iteration

	// ApplyBilinearFilterScreen(); // far too slow

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
		if (fPlayerTilt > - TILT_MAX)
			fPlayerTilt -= (fPlayerTiltSpeed)*fElapsedTime;
	}

	if (actionStates.lookDown)
	{
		if (fPlayerTilt <  TILT_MAX)
			fPlayerTilt += (fPlayerTiltSpeed)*fElapsedTime;
	}

	// Handle Movement
	if (actionStates.forward)
	{
		fPlayerX += cosf(fPlayerA) * fPlayerSpeed * fElapsedTime;
		fPlayerY += sinf(fPlayerA) * fPlayerSpeed * fElapsedTime;

		// collision detection - reversal of movement if hitting wall
		if (!(GetMapValue((int)fPlayerX, (int)fPlayerY, mapWalls) == 0))
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
		if (!(GetMapValue((int)fPlayerX, (int)fPlayerY, mapWalls) == 0))
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
		if (!(GetMapValue((int)fPlayerX, (int)fPlayerY, mapWalls) == 0))
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
		if (!(GetMapValue((int)fPlayerX, (int)fPlayerY, mapWalls) == 0))
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

void KablamGame3D::SetHorizontalWallCollisionValues(float rayAngle, float& yDistanceToWall, float& yTileHit, int& yWallType)
{
	// floats to hold ray x/y and then x/y offsets
	float fRayY{ 0.0f }, fRayX{ 0.0f }, fRayYO{ 0.0f }, fRayXO{ 0.0f };
	// int to store depth searched
	int nDepth{ 0 };
	// hit wall flag
	bool bHitWall = false;
	// pre-calculate value as used frequently
	float fTanValue{ tanf(rayAngle) };

	// Skip if looking left (PI) or right (0) as will never intersect a horizontal line
	if (rayAngle != 0.0f && rayAngle != PI)
	{
		if (rayAngle < PI) // looking down
		{
			fRayY = (int)fPlayerY + 1.0f;								// next y intersect moving down
			fRayX = fPlayerX + (fRayY - fPlayerY) / fTanValue;	// x position at next y intersect
			fRayYO = 1.0f;													// y offset down (1x1 grid)
			fRayXO = fRayYO / fTanValue;							// x offest for moving 1 y
		}
		else if (rayAngle > PI) // looking up
		{
			fRayY = (int)fPlayerY - 0.0001f;							// next y intersect moving up
			fRayX = fPlayerX + (fRayY - fPlayerY) / fTanValue;	// x position at next y intersect
			fRayYO = -1.0f;												// y offset up (1x1 grid)
			fRayXO = fRayYO / fTanValue;							// x offest for moving 1 y
		}

		// calculate HORIZONTAL 
		while (nDepth < MAX_DEPTH_OF_VIEW)
		{
			// integer values for vector used to test if hitting wall

			int nTestX{ 0 };
			if (fRayX < 0) // as (int)-0.XX will go to 0, not -1
				nTestX = (int)fRayX - 1;
			else
				nTestX = (int)fRayX;

			int nTestY = (int)fRayY;

			// test if ray has hit a wall
			if (WithinMapBounds(nTestX, nTestY))
			{
				if (GetMapValue(nTestX, nTestY, mapWalls) != 0)
				{
					bHitWall = true;
					yWallType = GetMapValue(nTestX, nTestY, mapWalls);
					nDepth = MAX_DEPTH_OF_VIEW;

					// find distance of wall 'hit' to 'left' side of tile
					if (fRayYO > 0) // ray looking down
						yTileHit = 1 - (fRayX - nTestX);
					else
						yTileHit = fRayX - nTestX;
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
				nDepth = MAX_DEPTH_OF_VIEW;
			}
		}

		// store distance to next HORIZONTAL line that is a wall
		if (bHitWall)
		{
			yDistanceToWall = RayLength(fPlayerX, fPlayerY, fRayX, fRayY);
		}

	} // end of HORIZONTAL line checking
}

void KablamGame3D::SetVerticalWallCollisionValues(float rayAngle, float& xDistanceToWall, float& xTileHit, int& xWallType)
{
	// floats to hold ray x/y and then x/y offsets
	float fRayY{ 0.0f }, fRayX{ 0.0f }, fRayYO{ 0.0f }, fRayXO{ 0.0f };
	// int to store depth searched
	int nDepth{ 0 };
	// hit wall flag
	bool bHitWall = false;
	// pre-calculate value as used frequently
	float fTanValue{ tanf(rayAngle) };

	// Skip if looking up (3*PI/2) or down PI/2 as will never intersect a vertical line
	if (rayAngle != P2 && rayAngle != P3)
	{
		// depth of field set to zero
		nDepth = 0;
		bool bHitWall = false;

		if (rayAngle > P3 || rayAngle < P2) // looking right
		{
			fRayX = ((int)(fPlayerX)+1.0f);								// next x intersect moving right
			fRayY = fPlayerY + (fRayX - fPlayerX) * fTanValue;	// y position at next x intersect
			fRayXO = 1;													// x offset down (1x1 grid)
			fRayYO = fRayXO * fTanValue;							// y offest for moving 1 x
		}
		else if (rayAngle > P2 && rayAngle < P3) // looking left
		{
			fRayX = (int)fPlayerX - 0.0001f;								// next x intersect moving left
			fRayY = fPlayerY + (fRayX - fPlayerX) * fTanValue;	// y position at next x intersect
			fRayXO = -1;												// x offset up (1x1 grid)
			fRayYO = fRayXO * fTanValue;							// y offest for moving 1 x
		}

		while (nDepth < MAX_DEPTH_OF_VIEW)
		{
			// integer values for vector used to test if hitting wall
			int nTestY{ 0 };
			if (fRayY < 0) // as (int)-0.XX will go to 0, not -1 
				nTestY = (int)fRayY - 1;
			else
				nTestY = (int)fRayY;

			int nTestX = (int)fRayX;


			// test if ray has hit a wall
			if (WithinMapBounds(nTestX, nTestY))
			{
				if (GetMapValue(nTestX, nTestY, mapWalls) != 0)
				{
					bHitWall = true;
					xWallType = GetMapValue(nTestX, nTestY, mapWalls);
					nDepth = MAX_DEPTH_OF_VIEW;

					// find distance of wall 'hit' to 'left' side of tile
					if (fRayXO < 0) // ray looking left
						xTileHit = 1 - (fRayY - nTestY);
					else
						xTileHit = fRayY - nTestY;
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
				nDepth = MAX_DEPTH_OF_VIEW;
			}
		}

		// store distance to next HORIZONTAL line that is a wall
		if (bHitWall)
		{
			xDistanceToWall = RayLength(fPlayerX, fPlayerY, fRayX, fRayY);
		}

	} // end of VERTICAL line checking
}

void KablamGame3D::SetHorizontalSurfaceHitCoords(int yColumn, float rayAngle, FloatCoord& hitCoords, COORD& indexCoords, bool lookingUp)
{
	float dy = yColumn - GetConsoleHeight() / 2.0f + fPlayerTilt;

	// handle dy == 0;
	if (dy == 0)
		dy = 1;

	// 'fisheye' correction
	float fRayFix = cosf(rayAngle - fPlayerA);

	if (lookingUp)
	{	
		// calculate ceiling textile 'hit' x & y - looking up so * fPlayerH - fWallHUnit
		hitCoords.X = fPlayerX + cosf(rayAngle) * (GetConsoleHeight() * fWallHUnit * (fPlayerH - fWallHUnit)) / dy / fRayFix;
		hitCoords.Y = fPlayerY + sinf(rayAngle) * (GetConsoleHeight() * fWallHUnit * (fPlayerH - fWallHUnit)) / dy / fRayFix;
	}
	else // looking down
	{
		// calculate floor tile 'hit' x & y - looking down so * fPlayerH
		hitCoords.X = fPlayerX + cosf(rayAngle) * (GetConsoleHeight() * fWallHUnit * fPlayerH) / dy / fRayFix;
		hitCoords.Y = fPlayerY + sinf(rayAngle) * (GetConsoleHeight() * fWallHUnit * fPlayerH) / dy / fRayFix;
	}


	// as casting a negative float to an integer truncates towards zero rather than continuing along scale.
	if (hitCoords.X < 0)
		indexCoords.X = (int)hitCoords.X - 1;
	else
		indexCoords.X = (int)hitCoords.X;

	if (hitCoords.Y < 0)
		indexCoords.Y = (int)hitCoords.Y - 1;
	else
		indexCoords.Y = (int)hitCoords.Y;
}


// get ray length from opposite and adjacent sides of ray vector
float KablamGame3D::RayLength(float px, float py, float rx, float ry) const
{
	// using a2 = sqrt(b2 + c2)
	float b = rx - px;
	float c = ry - py;

	return sqrt(b * b + c * c);
}

// check if within map bounds
bool KablamGame3D::WithinMapBounds(int x, int y) const
{
	return (x >= 0 && x < nMapWidth) && (y >= 0 && y < nMapHeight);
}

// get map value at coord x,y
int KablamGame3D::GetMapValue(int x, int y, const std::vector<int>& map) const
{
	if (WithinMapBounds(x, y))
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

short KablamGame3D::GetGlyphShadeByDistance(float distance)
{
	if (distance > 18) {
		return PIXEL_QUARTER;
	}
	else if (distance > 8) {
		return PIXEL_HALF;
	}
	else if (distance > 4) {
		return PIXEL_THREEQUARTERS;
	}
	else {
		return PIXEL_SOLID;
	}
}

int KablamGame3D::GetMipmapDetailLevel(float distance)
{
	if (distance > 18) {
		return 3;
	}
	else if (distance > 10) {
		return 2;
	}
	else if (distance > 6) {
		return 1;
	}
	else {
		return 0;
	}
}
