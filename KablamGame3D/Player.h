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

	void RotateRight(float elapsedTime);
	void RotateLeft(float elapsedTime);
	void LookUp(float elapsedTime);
	void LookDown(float elapsedTime);
	void MoveForward(float elapsedTime);
	void MoveBackward(float elapsedTime);
	void MoveRight(float elapsedTime);
	void MoveLeft(float elapsedTime);

	void StartJump();
	void Jumping(float elapsedTime);

	int GetScore();
	void IncreaseScore(int points);
	float GetHealth();
	int GetAmmo();

private:
	void TryMovement(float pdx, float pdy, float fElapsedTime);

	// manage player interaction with door
	void OpenDoorInFrontOfPlayer();

};

