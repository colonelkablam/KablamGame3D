#pragma once

#include <list>
#include <unordered_map>

// my library
#include "GameConstants.h"
#include "KablamEngine.h"
#include "SpriteObject.h"
#include "SpriteFactory.h"
#include "TextDisplay.h"
#include "SoundManager.h"
#include "Door.h"
#include "Player.h"


// this is the actual game class - inherits from Graphics Engine
// 
// -> need user to define OnUserCreate and OnUserUpdate as virtual in KGEngine
class KablamGame3D : public KablamEngine
{

private:
	const float fWallHUnit = 1.75f;
	const float FOV = PI / 4.0f;
	const int MAX_DEPTH_OF_VIEW = 32;

	Player* player;

	struct ActionStates
	{
		bool lookUp = false;
		bool lookDown = false;
		bool rotateLeft = false;
		bool rotateRight = false;
		bool rotateLeftSmall = false;
		bool rotateRightSmall = false;
		bool forward = false;
		bool backward = false;
		bool left = false;
		bool right = false;
		bool fire = false;
		bool use = false;
		bool jump = false;
		bool pause = false;
		bool toggleMap = false;
		bool toggleNextDisplay = false;
		bool togglePrevDisplay = false;
		bool startMusic = false;
		bool stopMusic = false;

	};

	ActionStates actionStates;

	int nMapDisplayStatus = 0;

	const std::wstring sSaveFolderName; // relative path to subDir 
	const std::wstring sSaveExtension;  // Kablam Game Save '.kgs'
	std::wstring sSaveFileName;

	// vectors to hold level data
	std::vector<int> mapWalls;
	std::vector<int> mapFloorTiles;
	std::vector<int> mapEnvironment; // this takes walls, floors, and doors (doors can update it), to create map of impassable world 'blocks'
	std::vector<int> mapCeilingTiles;
	std::vector<int> mapObjects;

	// map to hold doors
	std::map<std::pair<int, int>, Door*> doorContainer;

	// vectors to hold texture data
	std::vector<Texture*> wallTextures;
	std::vector<Texture*> floorTextures;
	std::vector<Texture*> ceilingTextures;
	std::vector<Texture*> spriteTextures;

	Texture* skyTexture;
	int xSkyOffset = 0;
	int ySkyOffset = 0;

	// crosshair
	int aimArray[25] = { 0, 0, 0, 0, 0,
						 1, 0, 0, 0, 1,
					  	 0, 1, 0, 1, 0,
						 1, 0, 0, 0, 1,
						 0, 0, 0, 0, 0, };

	Texture* spriteFloorLamp;
	Texture* spriteBarrel;
	Texture* spriteOctoBaddy;
	Texture* spriteOctoBaddyHit;
	Texture* spriteOctoBaddyDyingAni;
	Texture* spriteOctoBaddyDead;
	Texture* spriteFireball;
	Texture* spriteFireballHit;

	// map to hold sprite factories
	std::unordered_map<int, SpriteFactory*> spriteFactories;

	// list data structure used -> will be deleting/adding items regularly
	std::list<SpriteObject*> listSpriteObjects;

	// store the depth of each column
	std::vector <float> fDepthBuffers;

	// structure for float coordinates 
	struct FloatCoord {
		float X;
		float Y;
	};

	std::wstring headerText{ L"KamblamEngine3D" };

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

	// sound 
	SoundManager* soundManager;

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

	// Methods to initialize 
	void InitialiseFactories();

	void InitialiseEnvironmentMap();

	void SetPlayerStart(const std::vector<int>& floorPlan);

	void SetObjectsStart(const std::vector<int>& floorMap);

	void SetDoorMap(const std::vector<int>& wallMap);


	// handle player inputs
	void HandleKeyPress();

	// apply movements to player
	bool ApplyMovementAndActions(const float fElapsedTime);

	void OpenDoorInFrontOfPlayer();

	void SetHorizontalWallCollisionValues(float rayAngle, float& distanceToWall, float& tileHit, int& wallType, std::pair<int, int>& mapWallCoords);

	void SetVerticalWallCollisionValues(float rayAngle, float& distanceToWall, float& tileHit, int& wallType, std::pair<int, int>& mapWallCoords);

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


