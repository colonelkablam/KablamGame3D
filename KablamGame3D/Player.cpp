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

    // get appropriate buffer
    float xBuffer = pdx > 0 ? collisionBuffer : -collisionBuffer;
    float yBuffer = pdy > 0 ? collisionBuffer : -collisionBuffer;

    // Find new xy coords if move made including buffer
    float fNewX = xPos + xBuffer;
    float fNewY = yPos + yBuffer;

    // adjust for negative values as looking for grid index
    if (fNewX < 0)
        fNewX -= 1.0f;
    if (fNewY < 0)
        fNewY -= 1.0f;

    // get new XY index values if move made
    int newXIndex = static_cast<int>(fNewX);
    int newYIndex = static_cast<int>(fNewY);

    // Ensure the indices are within bounds before accessing the map
    if (newXIndex >= 0 && newXIndex < MAP_WIDTH &&
        newYIndex >= 0 && newYIndex < MAP_HEIGHT) {

        // find values
        int mapValueXDir = environmentMap->at(static_cast<int>(yPos) * MAP_WIDTH + newXIndex);
        int mapValueYDir = environmentMap->at(newYIndex * MAP_WIDTH + static_cast<int>(xPos));

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
