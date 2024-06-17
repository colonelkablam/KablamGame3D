#pragma once
#include <vector>

#include "GameConstants.h"

class Player
{
private:
	const int TILT_MAX = 50;

	float speed = 6.0f;
	float tilt = 0.0f;
	float rotationSpeed = 1.7f;
	float tiltSpeed = 170.0f;
	const float collisionBuffer = 0.2f;

	float xPos = 24.0f;
	float yPos = 24.0f;
	float facingAngle = 0;

	const float heightDefault = 0.7f;
	float height{ heightDefault };
	float upVelocity = 0.0f;
	float jumpPower = 6.0f;
	bool jumping = false;

	int score = 0;
	float health = 100.0f;
	int ammoAmount = 50;
	float timeElapsed = 0.0f;

	const std::vector<int>* environmentMap;

public:
	Player();
	~Player();

	// map for collisions, damage etc...
	void AddEnvironmentMap(std::vector<int>* newLevelEnvironment);
	float GetX();
	void SetX(float x);
	float GetY();
	void SetY(float y);
	float GetAngle();
	float GetSpeed();
	float GetTilt();
	float GetHeight();
	float GetHeightDefault();
	bool IsJumping();

	void UpdatePlayer(float timeStep);

	void RotateRight(float timeStep);
	void RotateLeft(float timeStep);
	void LookUp(float timeStep);
	void LookDown(float timeStep);
	void MoveForward(float timeStep);
	void MoveBackward(float timeStep);
	void MoveRight(float timeStep);
	void MoveLeft(float timeStep);

	void StartJump();
	void Jumping(float timeStep);

	int GetScore();
	void SetScore(int amount = 0);
	void IncreaseScore(int points);

	float GetHealth();
	void IncreaseHealth(float amount = 1.0f);
	void DecreaseHealth(float amount = 1.0f);

	int GetAmmo();
	void IncreaseAmmo(int amount = 1);
	void DecreaseAmmo(int amount = 1);

	float GetTime();

private:
	void TryMovement(float pdx, float pdy, float timeStep);

	// manage player interaction with door - done in game engine?
	//void OpenDoorInFrontOfPlayer();

};

