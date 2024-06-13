#include "Player.h"

Player::Player()
    : environmentMap{ nullptr }
{
}

Player::~Player()
{
}

void Player::AddEnvironmentMap(std::vector<int>* newLevelEnvironment)
{
        environmentMap = newLevelEnvironment;
}

float Player::GetX()
{
    return xPos;
}

void Player::SetX(float newX)
{
    xPos = newX;
}

void Player::SetY(float newY)
{
    yPos = newY;
}

float Player::GetY()
{
    return yPos;
}

float Player::GetAngle()
{
    return facingAngle;
}

float Player::GetSpeed()
{
    return speed;
}

float Player::GetTilt()
{
    return tilt;
}

float Player::GetHeight()
{
    return height;
}

float Player::GetHeightDefault()
{
    return heightDefault;
}

bool Player::IsJumping()
{
    return jumping;
}

void Player::RotateRight(float elspsedTime)
{
    facingAngle += rotationSpeed * elspsedTime;
    if (facingAngle >= PI2)
            facingAngle -= PI2;
}

void Player::RotateLeft(float elspsedTime)
{
    facingAngle -= rotationSpeed * elspsedTime;
    if (facingAngle < 0)
        facingAngle += PI2;
}

void Player::LookUp(float elapsedTime)
{
    if (tilt > -TILT_MAX)
        tilt -= tiltSpeed * elapsedTime;
}

void Player::LookDown(float elapsedTime)
{
    if (tilt < TILT_MAX)
        tilt += (tiltSpeed)*elapsedTime;
}

void Player::MoveForward(float elapsedTime)
{	
    // calculate changes in x & y
    float pdx = cosf(facingAngle);
    float pdy = sinf(facingAngle);

    TryMovement(pdx, pdy, elapsedTime);
}

void Player::MoveBackward(float elapsedTime)
{
    // calculate changes in x & y
    float pdx = -cosf(facingAngle);
    float pdy = -sinf(facingAngle);

    TryMovement(pdx, pdy, elapsedTime);

}

void Player::MoveRight(float elapsedTime)
{
    // calculate changes in x & y
    float pdx = cosf(facingAngle + PI / 2);
    float pdy = sinf(facingAngle + PI / 2);

    TryMovement(pdx, pdy, elapsedTime);
}

void Player::MoveLeft(float elapsedTime)
{
    // calculate changes in x & y
    float pdx = cosf(facingAngle - PI / 2);
    float pdy = sinf(facingAngle - PI / 2);

    TryMovement(pdx, pdy, elapsedTime);
}

// helper function
void Player::TryMovement(float pdx, float pdy, float fElapsedTime)
{	
    // apply movement to player x y
    float newX = xPos + pdx * speed * fElapsedTime;
    float newY = yPos + pdy * speed * fElapsedTime;

    if (newX * newY < MAP_WIDTH * MAP_HEIGHT); // if moving within map
    {
        // set appropriate collision buffer sign
        float xBuffer = pdx > 0 ? collisionBuffer : -collisionBuffer;
        float yBuffer = pdy > 0 ? collisionBuffer : -collisionBuffer;

        int mapValueXDir = environmentMap->at((int)yPos * MAP_WIDTH + (newX + xBuffer));
        int mapValueYDir = environmentMap->at((int)(newY + yBuffer) * MAP_WIDTH + xPos);
        
        // Check for collisions including the buffer before updating positions
        if (mapValueXDir == 0) {
            xPos = newX;
        }
        if (mapValueYDir == 0) {
            yPos = newY;
        }
    }
}

void Player::StartJump()
{
    if (!jumping)
    {
        upVelocity = jumpPower;
        jumping = true;
    }
}

void Player::Jumping(float elapsedTime)
{
    // jump mechanics
    if (jumping)
    {
        upVelocity += GRAVITY * elapsedTime;
        height += upVelocity * elapsedTime;

        if (height <= heightDefault)
        {
            jumping = false;
            height = heightDefault;
            upVelocity = 0.0f;
        }
    }
}

int Player::GetScore()
{
    return score;
}

void Player::IncreaseScore(int points)
{
    score += points;
}

float Player::GetHealth()
{
    return health;
}

int Player::GetAmmo()
{
    return ammoAmount;
}
