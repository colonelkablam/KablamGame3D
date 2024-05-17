#include "KablamGame3D.h"
#include <cmath>

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
	wallTextures.push_back(new Texture(L"./Textures/wall_main_32.txr"));
	wallTextures.push_back(new Texture(L"./Textures/wall_door_32.txr"));

	// Prepare floorTextures vector with size 10 and initialize with nullptr	// TBC for all structure texture vectors
	floorTextures.resize(10, nullptr);
	floorTextures.at(1) = new Texture(L"./Textures/floor_main_32.txr");
	floorTextures.at(2) = new Texture(L"./Textures/floor_lava_32.txr");
	floorTextures.at(8) = new Texture(L"./Textures/floor_start_32.txr");
	floorTextures.at(9) = new Texture(L"./Textures/floor_finish_32.txr");


	ceilingTextures.push_back(nullptr); // position 0 will return nullptr
	ceilingTextures.push_back(new Texture(L"./Textures/ceiling_main_32.txr"));
	ceilingTextures.push_back(new Texture(L"./Textures/ceiling_light_32.txr"));

	AddToLog(L"Map textures added...");

	// Sprite textures
	spriteFloorLamp = new Texture(L"./Textures/Sprites/sprite_lamp_32.txr");
	spriteOctoBaddy1 = new Texture(L"./Textures/Sprites/sprite_octo_32_1.txr");
	spriteOctoBaddy2 = new Texture(L"./Textures/Sprites/sprite_octo_32_2.txr");


	AddToLog(L"Sprite textures added...");

	// Enviroment textures
	skyTexture = new Texture(L"./Textures/Environments/sky_2.txr");

	fDepthBuffers.resize(GetConsoleWidth(), 1000.0f);

	SetPlayerStart(mapFloorTiles);
	SetObjectsStart(mapObjects);

	SetResizeWindowLock(true);
	SetConsoleFocusPause(true);
	SetWindowPosition(100, 100);

	return true;
}

bool KablamGame3D::OnGameUpdate(float fElapsedTime)
{
	HandleKeyPress();

	ApplyMovementAndActions(fElapsedTime);

	for (auto& object : listObjects)
	{
		object.UpdateSprite(fElapsedTime);
	}

	// get the display setting before rendering the screen
	DisplayState displaySetting = displayManager.GetDisplay();

	// calculate ratio to multiply screen column by to get ray angle
	float fAngleIncrement = FOV / static_cast<float>(GetConsoleWidth());
	float fStartingAngle = fPlayerA - FOV / 2.0f;

	UpdateSkyView(); // keeps the calculations out of the screen columns loop, only needed per frame

	// reset the depth buffers
	std::fill(fDepthBuffers.begin(), fDepthBuffers.end(), 1000.0f);

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


		// threading experiment - doesn't seem to add value
		// 
		//std::thread horizontalThread([this, fRayAngle, &fYDistanceToWall, &fYTileHit, &nWallTypeY]() {
		//	this->SetHorizontalWallCollisionValues(fRayAngle, fYDistanceToWall, fYTileHit, nWallTypeY);
		//	});

		//std::thread verticalThread([this, fRayAngle, &fXDistanceToWall, &fXTileHit, &nWallTypeX]() {
		//	this->SetVerticalWallCollisionValues(fRayAngle, fXDistanceToWall, fXTileHit, nWallTypeX);
		//	});

		//// Wait for both threads to complete their execution
		//horizontalThread.join();
		//verticalThread.join();


		// decalare variables to store final results
		float fDistanceToWall{ 1000.0f };
		float fTileHit{ -1.0f };
		int nWallType{ 0 };
		bool bHitXWall{ false };

		// get shortist distance and correlating tile hit axis and wall type
		if (fYDistanceToWall <= fXDistanceToWall)
		{
			// bHitXWall = false; // record X-axis boundry hit - no need to implement as already false
			fDistanceToWall = fYDistanceToWall;
			fTileHit = fYTileHit;
			nWallType = nWallTypeY;
		}
		else
		{
			bHitXWall = true; // record X-axis boundry hit
			fDistanceToWall = fXDistanceToWall;
			fTileHit = fXTileHit;
			nWallType = nWallTypeX;
		}

		// add before fish-eye!! this gives 'true' distance
		fDepthBuffers.at(x) = fDistanceToWall;


		// correct for fish-eye effect
		fDistanceToWall *= cosf(fRayAngle - fPlayerA);



		// get ratios of wall to ceiling and floor
		// - how much of the column to draw as ceiling/wall/floor

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
				CHAR_INFO pixel{ PIXEL_SOLID, FG_BLUE };
				//pixel = skyTexture->GetPixel((int)x/2, (int)y/3);

				//pixel.Attributes += BG_BLUE;

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

				// texture to use
				int nCeilingType{ GetMapValue(ceilingHitIndex.X, ceilingHitIndex.Y, mapCeilingTiles) };

				if (nCeilingType != 0)
				{
					// render pixel according to current display setting
					SetRenderPixel(pixel, ceilingTextures.at(nCeilingType), fXTextureTileHit, fYTextureTileHit, fDistanceToCeiling, displaySetting);
					DrawPoint(x, y, pixel);
				}
				else
				{
					DisplaySky(x, y);
				}
				

			}
			// draw a WALL character
			else if (y > nCeiling && y <= nFloor && nWallType != 0)
			{
				// default wall pixel
				CHAR_INFO pixel;
				pixel.Attributes = FG_RED | BG_CYAN;
				pixel.Char.UnicodeChar = PIXEL_HALF;

				// calculate Y sample of texture tile (ratio of y value and wall height in pixels)
				float fSampleY = ((float)y - (float)nCeiling) / ((float)nFloor + 1 - (float)nCeiling );

				// render pixel according to current display setting
				SetRenderPixel(pixel, wallTextures.at(nWallType), fTileHit, fSampleY, fDistanceToWall, displaySetting, bHitXWall);

				DrawPoint(x, y, pixel);

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

				// render pixel according to current display setting
				SetRenderPixel(pixel, GetTexturePointer(floorTextures, nFloorType), fXTextureTileHit, fYTextureTileHit, fDistanceToFloor, displaySetting);

				DrawPoint(x, y, pixel);

			}

		} // end of column
	} // end of screen column iteration

	// ApplyBilinearFilterScreen(); // far too slow

	DisplayObjects();
	DisplayAim();
	DisplayMap(5, 5, 5);
	DisplayScore();

	return true;
} // end of OnGameUpdate


// member methods of derived KablamGame class



// key press actions

void KablamGame3D::SetPlayerStart(const std::vector<int>& floorMap)
{
	size_t xPosStart{ 0 };
	size_t yPosStart{ 0 };
	size_t xPosFinish{ 0 };
	size_t yPosFinish{ 0 };

	for (size_t x{ 0 }; x < nMapWidth; ++x)
	{
		for (size_t y{ 0 }; y < nMapWidth; ++y)
		{
			// Determine the index based on the 2D coordinates
			int tile = floorMap[y * nMapWidth + x];
			if (tile == 8)
			{
				xPosStart = x;
				yPosStart = y;
			}
			else if (tile == 9)
			{
				xPosFinish = x;
				yPosFinish = y;
			}
			// else do nothing
		}
	}

	// Assign the starting position to the player
	fPlayerX = static_cast<float>(xPosStart + 0.5f);
	fPlayerY = static_cast<float>(yPosStart + 0.5f);
}

void KablamGame3D::SetObjectsStart(const std::vector<int>& floorMap)
{
	for (size_t x{ 0 }; x < nMapWidth; ++x)
	{
		for (size_t y{ 0 }; y < nMapWidth; ++y)
		{
			// Determine the index based on the 2D coordinates
			int tile = floorMap[y * nMapWidth + x];
			if (tile == 1)
			{
				listObjects.push_back({ x + 0.5f , y + 0.5f, 1.0f, 0, false, false, spriteOctoBaddy1, spriteOctoBaddy2
					});
			}
			else if (tile == 2)
			{
				listObjects.push_back({x + 0.5f, y + 0.5f, 0.0f, 0, false, true, spriteFloorLamp });
			}
			// else do nothing
		}
	}

}

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
	actionStates.toggleNextDisplay = GetKeyState(VK_OEM_PERIOD).bPressed;
	actionStates.togglePrevDisplay = GetKeyState(VK_OEM_COMMA).bPressed;


}

bool KablamGame3D::ApplyMovementAndActions(const float fElapsedTime)
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
		// calculate changes in x & y
		float pdx = cosf(fPlayerA);
		float pdy = sinf(fPlayerA);

		TryMovement(pdx, pdy, fElapsedTime);
	}

	if (actionStates.backward)
	{
		// calculate changes in x & y
		float pdx = -cosf(fPlayerA);
		float pdy = -sinf(fPlayerA);

		TryMovement(pdx, pdy, fElapsedTime);
	}

	if (actionStates.right)
	{
		// calculate changes in x & y
		float pdx = cosf(fPlayerA + PI / 2);
		float pdy = sinf(fPlayerA + PI / 2);

		TryMovement(pdx, pdy, fElapsedTime);
	}

	if (actionStates.left)
	{
		// calculate changes in x & y
		float pdx = cosf(fPlayerA - PI / 2);
		float pdy = sinf(fPlayerA - PI / 2);

		TryMovement(pdx, pdy, fElapsedTime);
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


	// display selection
	if (actionStates.toggleNextDisplay)
	{
		displayManager.SetNextDisplay();
		DisplayAlertMessage(L"Display set to: " + displayManager.DisplayStateToString());

	}
	if (actionStates.togglePrevDisplay)
	{
		displayManager.SetPreviousDisplay();
		DisplayAlertMessage(L"Display set to: " + displayManager.DisplayStateToString());

	}

	
	// pause
	if (actionStates.pause)
		DisplayAlertMessage(L"Game Paused.");

	return true;
}

// helper function
void KablamGame3D::TryMovement(float pdx, float pdy, float fElapsedTime)
{		// set appropriate collision buffer sign
	float xBuffer = pdx > 0 ? playerCollisionBuffer : -playerCollisionBuffer;
	float yBuffer = pdy > 0 ? playerCollisionBuffer : -playerCollisionBuffer;

	// apply movement to player x y
	float newX = fPlayerX + pdx * fPlayerSpeed * fElapsedTime;
	float newY = fPlayerY + pdy * fPlayerSpeed * fElapsedTime;

	// Check for collisions including the buffer before updating positions
	if (GetMapValue((int)(newX + xBuffer), (int)fPlayerY, mapWalls) == 0) {
		fPlayerX = newX;
	}

	if (GetMapValue((int)fPlayerX, (int)(newY + yBuffer), mapWalls) == 0) {
		fPlayerY = newY;
	}

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

void KablamGame3D::SetRenderPixel(CHAR_INFO& pixel, const Texture* textureToRender, const float textureTileXHit, const float textureTileYHit, const float distanceToHit, const DisplayState displaySetting, const bool hitXWall)
{	
	// draw corresponding pixel per ceiling tile
	if (textureToRender == nullptr) // handle nullptr by not changing default pixel
		return;

	//DrawPoint(x, y, colour, nCeilingShadeGlyph);
	if (displaySetting == DisplayState::NORMAL)
	{
		pixel = textureToRender->SamplePixel(textureTileXHit, textureTileYHit);
	}

	else if (displaySetting == DisplayState::DISTANCE_SHADING)
	{
		// char to draw 'shade'
		pixel.Char.UnicodeChar = GetGlyphShadeByDistance(distanceToHit);
		// colour
		pixel.Attributes = textureToRender->SampleColour(textureTileXHit, textureTileYHit);
	}
	else if (displaySetting == DisplayState::ANGLE_SHADING) // same as distance but 
	{
		// Determine the glyph shade based on distance
		short glyph = GetGlyphShadeByDistance(distanceToHit);

		// Modify the glyph further based on whether it hit an X or Y wall
		if (hitXWall)
		{
			if (glyph == PIXEL_QUARTER) glyph = PIXEL_HALF;
			else if (glyph == PIXEL_HALF) glyph = PIXEL_THREEQUARTERS;
			else if (glyph == PIXEL_THREEQUARTERS) glyph = PIXEL_SOLID;

			else glyph = PIXEL_SOLID;
		}

		pixel.Char.UnicodeChar = glyph;

		// colour
		pixel.Attributes = textureToRender->SampleColour(textureTileXHit, textureTileYHit);
	}
	else if (displaySetting == DisplayState::LINEAR_INT)
	{
		pixel = textureToRender->LinearInterpolationWithGlyphShading(textureTileXHit, textureTileYHit);
	}
	else if (displaySetting == DisplayState::MIP_MAP)
	{
		// detail level (if mipmapping used)
		int nDetailLevel{ GetMipmapDetailLevel(distanceToHit) };

		pixel = textureToRender->SamplePixelWithMipmap(textureTileXHit, textureTileYHit, nDetailLevel);
	}
	else if (displaySetting == DisplayState::PRE_RENDERED)
	{
		// TBC
	}
	else // default pixel used
	{
		return;
	}

	// if texture has an illumination value greater than 0 make it as 'bright' as possible with full pixel
	if (textureToRender->GetIllumination() != 0)
		pixel.Char.UnicodeChar = PIXEL_SOLID;

}

// Helper function to safely get a texture pointer
const Texture* KablamGame3D::GetTexturePointer(const std::vector<Texture*>& textures, int index)
{
	// Ensure the index is within bounds
	if (index >= 0 && static_cast<size_t>(index) < textures.size())
	{
		return textures[index];
	}
	return nullptr; // Return nullptr if index is out of bounds
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

void KablamGame3D::DisplayObjects()
{
	// calculate all distances
	for (auto& object : listObjects)
		object.distFromPlayer = RayLength(fPlayerX, fPlayerY, object.x, object.y);

	// Sort the list by distance from the player (furthest first)
	listObjects.sort([this](const sObject& a, const sObject& b) {
		// Directly compare precomputed distances
		return a.distFromPlayer > b.distFromPlayer;
	});

	for (const auto& object : listObjects)
	{
		// Can object be seen?
		// realative position
		float fVecX = object.x - fPlayerX;
		float fVecY = object.y - fPlayerY;
		float fDistanceFromPlayer = object.distFromPlayer;

		// Rotate around screen
		float fEyeX = cosf(fPlayerA);
		float fEyeY = sinf(fPlayerA);

		// Calculate angle between object and players view direction
		// relative angle from player to object = (angle x-axis to object) - (angle x-axis to look dir)
		float fObjectAngle = atan2f(fVecY, fVecX) - atan2f(fEyeY, fEyeX);
		// keeps angle to between -PI and PI
		if (fObjectAngle < -PI) fObjectAngle += 2.0f * PI;
		if (fObjectAngle > PI) fObjectAngle -= 2.0f * PI;

		// within field of view
		bool bInPlayerFOV = fabs(fObjectAngle) < FOV / 2.0f;

		if (bInPlayerFOV && fDistanceFromPlayer >= 0.5f && fDistanceFromPlayer < 30.0f)
		{
			int spriteWidth = object.sprite->GetWidth();
			int spriteHeight = object.sprite->GetHeight();

			// calculate objects height, top, bottom, all with respect to the screen Y position (very similar to the wall height calculation)
			float fObjectHeight = (GetConsoleHeight() / fDistanceFromPlayer) * fWallHUnit;
			// takes into account the players view tilt and jump height
			float fObjectTop{ GetConsoleHeight()/2.0f - (fObjectHeight/2 * (fWallHUnit - fPlayerHDefault -  fPlayerH)*2) - fPlayerTilt};
			// account for object z height off floor
			float verticalAdjustment = ((spriteHeight/2 - spriteHeight * object.z) * fWallHUnit) / fDistanceFromPlayer;
			fObjectTop += verticalAdjustment;

			float fObjectBottom{ fObjectTop + fObjectHeight };
			float fObjectAspectRatio = (float)spriteHeight / (float)spriteWidth;
			float fObjectWidth = fObjectHeight / fObjectAspectRatio - 2;

			// where is object on screen x-axis
			float fMiddleOfObject = (0.5f * (fObjectAngle / (FOV / 2.0f)) + 0.5f) * (float)GetConsoleWidth();

			// Draw object
			for (float lx = 0; lx < fObjectWidth; lx ++)
			{
				// Calculate the X position on the screen
				int screenX = (int)(fMiddleOfObject + lx - fObjectWidth / 2.0f);

				for (float ly = 0; ly < fObjectHeight; ly++)
				{
					// Calculate the Y position on the screen
					int screenY = (int)(fObjectTop + ly);

					// Ensure screenX is within the bounds of the screen and depth buffer
					if (screenX >= 0 && screenX < GetConsoleWidth())
					{
						// Check if the object is closer than the wall previously drawn at this screenX
						if (fDistanceFromPlayer <= fDepthBuffers[screenX])
						{
							// Calculate the corresponding position in the sprite
							int spriteX = (int)(lx * spriteWidth / fObjectWidth);
							int spriteY = (int)(ly * spriteHeight / fObjectHeight);

							// check if 'transparent'
							short glyph = object.sprite->GetGlyph(spriteX, spriteY);

							// Check if the pixel is not transparent (assuming some alpha value defines transparency)
							if (glyph != L' ' && fDistanceFromPlayer <= fDepthBuffers[screenX])
							{
								// Get the color from the sprite at this position
								short colour = object.sprite->GetColour(spriteX, spriteY);
								// adjust glyph for shading
								if (!object.illuminated)
									glyph = GetGlyphShadeByDistance(fDistanceFromPlayer);

								// Draw pixel at the calculated screen position

								DrawPoint(screenX, screenY, colour, glyph);
								fDepthBuffers[screenX] = fDistanceFromPlayer; // add to depth buffer
							}
						}
					}
				}
			}
		}
	}
}

void KablamGame3D::UpdateSkyView()
{
	// angleScale is factor for angle to texture width mapping
	float angleScale = skyTexture->GetWidth() / FOV * 2.6f;

	xSkyOffset = static_cast<int>(fPlayerA * angleScale) % skyTexture->GetWidth();
	ySkyOffset = skyTexture->GetHeight() - GetConsoleHeight() / 2 + fPlayerTilt;
}

void KablamGame3D::DisplaySky(int x, int y)
{
	int adjustedY = y + ySkyOffset;

	if (adjustedY < 0 || adjustedY >= skyTexture->GetHeight())
		DrawPoint(x, y, FG_DARK_BLUE);
	else
		DrawPoint(x, y, skyTexture->GetColour((x + xSkyOffset) % skyTexture->GetWidth(), y + ySkyOffset));
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

// display map and player
void KablamGame3D::DisplayMap(int xPos, int yPos, int scale) {
	if (nMapDisplayStatus != 0) {
		for (int x{ 0 }; x < nMapWidth; x++) {
			for (int y{ 0 }; y < nMapHeight; y++) {
				short wcMapPixelColour = FG_BLACK;
				if (mapWalls[y * nMapWidth + x] == 1) {
					wcMapPixelColour = FG_YELLOW;
				}

				// Calculate the actual screen position based on the starting point (xPos, yPos)
				int screenX = xPos + (x * scale);
				int screenY = yPos + (y * scale);

				// Draw top partial map or an enlarged map based on nMapDisplayStatus
				DrawPoint(screenX + (int)(scale/2), screenY + (int)(scale / 2), wcMapPixelColour, PIXEL_SOLID);

				if (nMapDisplayStatus > 1) {
					// Draw full map
					DrawRectangleEdgeLength(screenX, screenY, scale, scale, wcMapPixelColour, true);
				}
			}
		}

		// Draw player position and orientation
		int pX = xPos + (int)(fPlayerX * scale);
		int pY = yPos + (int)(fPlayerY * scale);
		DrawLine(pX, pY, pX + cosf(fPlayerA) * scale*2, pY + sinf(fPlayerA) * scale*2, FG_DARK_RED);
		DrawPoint(pX, pY, FG_WHITE, PIXEL_SOLID);
	}
}

void KablamGame3D::DisplayScore()
{
	std::wstring text{ L"KamblamEngine3D @ . ?" };
	textDisplay.DisplayString(*this, 10, nScreenHeight - 10, text, FG_DARK_BLUE, PIXEL_SOLID);
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
	if (distance > 36) {
		return 3;
	}
	else if (distance > 24) {
		return 2;
	}
	else if (distance > 12) {
		return 1;
	}
	else {
		return 0;
	}
}
