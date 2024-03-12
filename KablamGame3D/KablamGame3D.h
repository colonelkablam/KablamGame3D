#pragma once

// my library
#include "KablamEngine.h"

// this is the actual game class - inherits from Graphics Engine
// 
// -> need user to define OnUserCreate and OnUserUpdate as virtual in KGEngine
class KablamGame3D : public KablamEngine
{

	// member attributes for KablamEngine
private:
	const int nMapWidth = 32;
	const int nMapHeight = 32;
	const float fWallHUnit = 1.5f;

	const float PI = 3.14159f;
	const float P2 = PI / 2.0f;
	const float P3 = PI * (3.0f / 2.0f);

	const std::wstring sSaveFolderName; // relative path to subDir 
	const std::wstring sSaveExtension;  // Kablam Game Save '.kgs'
	std::wstring sSaveFileName;

	// vectors to hold level data
	std::vector<int> mapWalls;
	std::vector<int> mapFloorTiles;
	std::vector<int> mapCeilingTiles;

	// vectors to hold sprite data
	std::vector<Texture*> wallTextures{};
	std::vector<Texture*> floorTextures{};
	std::vector<Texture*> ceilingTextures{};
	std::vector<Texture*> spriteTextures{};

	//// crosshair
	//int aimArray[25] = {0, 0, 1, 0, 0,
	//					0, 0, 1, 0, 0,
	//					1, 1, 1, 1, 1,
	//					0, 0, 1, 0, 0,
	//					0, 0, 1, 0, 0, };

	// crosshair
	int aimArray[25] = { 0, 0, 0, 0, 0,
						 1, 0, 0, 0, 1,
					  	 0, 1, 0, 1, 0,
						 1, 0, 0, 0, 1,
						 0, 0, 0, 0, 0, };


	const float FOV = PI / 4.0f;
	const int MAX_DEPTH_OF_VIEW = 32;

	float fPlayerSpeed = 6.0f;
	float fPlayerRotationSpeed = 1.7f;
	float fPlayerTilt = 0.0f;
	float fPlayerTiltSpeed = 170.0f;
	const int TILT_MAX = 40;

	struct ActionStates
	{
		bool lookUp = false;
		bool lookDown = false;
		bool rotateLeft = false;
		bool rotateRight = false;
		bool forward = false;
		bool backward = false;
		bool left = false;
		bool right = false;
		bool use = false;
		bool jump = false;
		bool pause = false;
		bool toggleMap = false;
	};

	ActionStates actionStates;

	COORD newMouseCoords = { 0,0 };
	COORD centerScreenCoords = { 0,0 };
	std::pair<int, int> mouseVelocity = { 0.0f, 0.0f };

	struct FloatCoord {
		float X;
		float Y;
	};

	float fPlayerX = 24.0f;
	float fPlayerY = 24.0f;
	float fPlayerA = 0;

	float fPlayerHDefault = 0.6f;
	float fPlayerH{ fPlayerHDefault };
	float fPlayerUpVelocity = 0.0f;
	float fGravity = -19.0f;
	float fPlayerJumpPower = 6.0f;
	bool bPlayerJumping = false;

	int nMapDisplayStatus = 0;


public:

	// constructor
	KablamGame3D(std::wstring title = L"New Unnamed Game");

	// destructor
	~KablamGame3D();

	// member methods for KablamGame

		// virtual methods to be defined
	bool OnGameCreate();

	bool OnGameUpdate(float fElapsedTime);

private:
	// handle player inputs
	void HandleKeyPress();

	// apply movements to player
	bool ApplyMovementAndActions(float fElapsedTime);

	void SetHorizontalWallCollisionValues(float rayAngle, float& yDistanceToWall, float& yTileHit, int& yWallType);

	void SetVerticalWallCollisionValues(float rayAngle, float& yDistanceToWall, float& yTileHit, int& yWallType);	

	void SetHorizontalSurfaceHitCoords(int yColumn, float rayAngle, FloatCoord& hitCoords, COORD& indexCoords, bool lookingUp);

	// get ray length from opposite and adjacent sides of ray vector
	float RayLength(float px, float py, float rx, float ry) const;

	// check if within map bounds
	bool WithinMapBounds(int x, int y) const;

	// get map value at coord x,y
	int GetMapValue(int x, int y, const std::vector<int>& map) const;

	void DisplayAim(short colour = FG_WHITE, short glyph = PIXEL_SOLID);

	void DisplayMap(int xPos, int yPos, int scale);

	short GetGlyphShadeByDistance(float distance);

	int GetMipmapDetailLevel(float distance);


}; // end of KablamGraphics class definition


