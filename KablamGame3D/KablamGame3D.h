#pragma once

#include <list>

// my library
#include "KablamEngine.h"
#include "TextDisplay.h"

// this is the actual game class - inherits from Graphics Engine
// 
// -> need user to define OnUserCreate and OnUserUpdate as virtual in KGEngine
class KablamGame3D : public KablamEngine
{
	// member attributes for KablamEngine
public:
	static const float PI;
	static const float P2;
	static const float P3;

private:
	const int nMapWidth = 32;
	const int nMapHeight = 32;
	const float fWallHUnit = 1.75f;

	const std::wstring sSaveFolderName; // relative path to subDir 
	const std::wstring sSaveExtension;  // Kablam Game Save '.kgs'
	std::wstring sSaveFileName;

	// vectors to hold level data
	std::vector<int> mapWalls;
	std::vector<int> mapFloorTiles;
	std::vector<int> mapCeilingTiles;
	std::vector<int> mapObjects;

	// vectors to hold texture data
	std::vector<Texture*> wallTextures;
	std::vector<Texture*> floorTextures;
	std::vector<Texture*> ceilingTextures;
	std::vector<Texture*> spriteTextures;

	Texture* skyTexture;
	int xSkyOffset = 0;
	int ySkyOffset = 0;

	// Adjust 'angleScale' to tune how sensitive the texture offset is to player rotation
	// Example: Assuming you want the texture to span a 90-degree field of view
	//const float fieldOfViewRadians = 90 * (PI / 180); // Convert degrees to radians

	// crosshair
	int aimArray[25] = { 0, 0, 0, 0, 0,
						 1, 0, 0, 0, 1,
					  	 0, 1, 0, 1, 0,
						 1, 0, 0, 0, 1,
						 0, 0, 0, 0, 0, };

	Texture* spriteFloorLamp;
	Texture* spriteOctoBaddy;


	std::vector <float> fDepthBuffers;

	// simple object to represent a game object
	struct sObject {
		float x;
		float y;
		float z; // height off floor
		float distFromPlayer;
		float angle;
		float relativeAngle;
		int width;
		int height;
		int type;
		bool dead;
		bool illuminated;
		Texture* currentSprite;
		Texture* aliveSprite;
		Texture* deadSprite;

		// default Constructor
		sObject(float initX = 0.0f, float initY = 0.0f, float initZ = 0.0f, int initType = 0, bool isDead = false,
			bool isIlluminated = false, Texture* initSpriteAlive = nullptr, Texture* initSpriteDead = nullptr)
			: x(initX), y(initY), z(initZ), type(initType), dead(isDead), illuminated(isIlluminated),
			currentSprite{ initSpriteAlive }, aliveSprite(initSpriteAlive), deadSprite(initSpriteDead), distFromPlayer{ 1000.f }, angle{ 0.0f },
			width{ currentSprite->GetWidth() / 4 }, height{ currentSprite->GetHeight() / 2 }, relativeAngle{ 0.0f }
		{}

		void UpdateSprite(float timePassed, float playerX, float playerY)
		{
			float dx = x - playerX;
			float dy = y - playerY;

			// Calculate the angle from the player to the sprite
			float angleToPlayer = atan2(dy, dx);

			// Calculate the relative angle taking into account the sprite's angle
			relativeAngle = angle - angleToPlayer;


			if (relativeAngle < 0)
			{
				relativeAngle += 2 * PI;
			}

		}

		CHAR_INFO GetPixel(int x, int y)
		{
			int xOffset = 0;
			int yOffset = 0;

			const float SEGMENT_ANGLE = (2*PI) / 8; // Each segment covers 45 degrees or PI/4 radians

			// Normalize the relativeAngle to be within the range [0, 2*PI)
			float normalizedAngle = relativeAngle;
			if (normalizedAngle < 0)
			{
				normalizedAngle += 2*PI;
			}

			// Determine the view index
			int viewIndex = static_cast<int>((relativeAngle + SEGMENT_ANGLE/2) / SEGMENT_ANGLE);

			// Calculate offsets based on the view index
			xOffset = (viewIndex % 4) * 32; // 4 views per row
			yOffset = (viewIndex / 4) * 32; // Move to next row for views 4-7

			return currentSprite->GetPixel(x + xOffset, y + yOffset);
		}

		~sObject()
		{
			//delete sprite handled by KablamGame3D;
		}

	};

	// list of objects as will be deleting items within
	std::list<sObject> listObjects;

	const float FOV = PI / 4.0f;
	const int MAX_DEPTH_OF_VIEW = 32;

	float fPlayerSpeed = 6.0f;
	float fPlayerRotationSpeed = 1.7f;
	float fPlayerTilt = 0.0f;
	float fPlayerTiltSpeed = 170.0f;
	const int TILT_MAX = 45;
	const float playerCollisionBuffer = 0.2f;

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
		bool toggleNextDisplay = false;
		bool togglePrevDisplay = false;

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

	float fPlayerHDefault = 0.7f;
	float fPlayerH{ fPlayerHDefault };
	float fPlayerUpVelocity = 0.0f;
	float fGravity = -19.0f;
	float fPlayerJumpPower = 6.0f;
	bool bPlayerJumping = false;

	int nMapDisplayStatus = 0;

	enum DisplayState {
		NORMAL,
		DISTANCE_SHADING,
		ANGLE_SHADING,
		LINEAR_INT,
		MIP_MAP,
		PRE_RENDERED, // ?
		DISPLAY_STATE_COUNT // Automatically gives us the count of states
	};

	struct DisplayManager
	{
		DisplayState currentDisplay = DisplayState::NORMAL;

		DisplayState GetDisplay()
		{
			return currentDisplay;
		}

		void SetDisplay(DisplayState newDisplay)
		{
			currentDisplay = newDisplay;
		}

		void SetNextDisplay()
		{
			currentDisplay = static_cast<DisplayState>((currentDisplay + 1) % DISPLAY_STATE_COUNT);
		}

		void SetPreviousDisplay()
		{
			currentDisplay = static_cast<DisplayState>((currentDisplay - 1) % DISPLAY_STATE_COUNT);
		}

		std::wstring DisplayStateToString() {
			switch (currentDisplay) {
			case NORMAL: return L"NORMAL";
			case DISTANCE_SHADING: return L"DISTANCE_SHADING";
			case ANGLE_SHADING: return L"ANGLE_SHADING";
			case LINEAR_INT: return L"LINEAR_INT";
			case MIP_MAP: return L"MIP_MAP";
			case PRE_RENDERED: return L"PRE_RENDERED";
			default: return L"UNKNOWN";
			}
		}

	};

	// manage the rendering state
	DisplayManager displayManager;

	// for displaying text to the screen
	TextDisplay textDisplay;


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
	void SetPlayerStart(const std::vector<int>& floorPlan);

	void SetObjectsStart(const std::vector<int>& floorMap);

	// handle player inputs
	void HandleKeyPress();

	// apply movements to player
	bool ApplyMovementAndActions(const float fElapsedTime);

	void TryMovement(float pdx, float pdy, float fElapsedTime);

	void SetHorizontalWallCollisionValues(float rayAngle, float& yDistanceToWall, float& yTileHit, int& yWallType);

	void SetVerticalWallCollisionValues(float rayAngle, float& yDistanceToWall, float& yTileHit, int& yWallType);	

	void SetHorizontalSurfaceHitCoords(int yColumn, float rayAngle, FloatCoord& hitCoords, COORD& indexCoords, bool lookingUp);

	void SetRenderPixel(CHAR_INFO& pixel, const Texture* textureToRender, const float textureTileXHit, const float textureTileYHit, const float distanceToHit, const DisplayState displaySetting, const bool hitXWall = false);

	const Texture* GetTexturePointer(const std::vector<Texture*>& textures, int index);

	// get ray length from opposite and adjacent sides of ray vector
	float RayLength(float px, float py, float rx, float ry) const;

	// check if within map bounds
	bool WithinMapBounds(int x, int y) const;

	// get map value at coord x,y
	int GetMapValue(int x, int y, const std::vector<int>& map) const;

	void DisplayObjects();

	// uses player viewing angle and tilt to update sky display offsets
	void UpdateSkyView();

	void DisplaySky(int x, int y);

	void DisplayAim(short colour = FG_WHITE, short glyph = PIXEL_SOLID);

	void DisplayMap(int xPos, int yPos, int scale);

	void DisplayScore();

	short GetGlyphShadeByDistance(float distance);

	int GetMipmapDetailLevel(float distance);


}; // end of KablamGraphics class definition


