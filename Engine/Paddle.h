#pragma once

#include "Ball.h"
#include "Vec2.h"
#include "RectF.h"
#include "Colors.h"
#include "Graphics.h"
#include "Keyboard.h"
#include <chrono>

class Paddle
{
public:
    Paddle() = default;
    Paddle( const Vec2& pos_in, bool easy = false );
    void Draw( Graphics& gfx ) const;
    void DrawAsLifesRemaining( Graphics& gfx, const int lifesRemaining, const Vec2& pos, const float sizeRatio = 0.5f ) const;
    bool DoBallCollision( Ball& ball );
    void DoWallCollision( const RectF& walls );
    void Update( const Keyboard& kbd, float dt );
    RectF GetRect() const;
    RectF GetLeftGunPosition() const;
    RectF GetRightGunPosition() const;

    // power ups
    void IncreaseSize( const float duration );
    void AddLaserGun( const float duration );
    bool HasLaserGun() const;
private:
    void CalcLaserPositions();
    static constexpr Color wingColor = { 210, 33, 33 };// Colors::Red;
    static constexpr Color color = Colors::White;
    static constexpr float wingWidth = 18.0f;
    
    float speed = 550.0f;

    // control the paddle rebound behavior here
    static constexpr float fixedZoneWidthRatio = 0.1f;
public:
    static constexpr float maximumExitRatio = 2.6f; // made it public to be able to use it in multiBallCreation
private:
    Surface surf = Surface::FromFile( L"Images\\paddle.png" );
    Surface surfBig = Surface::FromFile( L"Images\\paddleBig.png" );
    float halfWidth = surf.GetWidth() / 2.0f;
    float halfHeight = surf.GetHeight() / 2.0f;

    // these are derived from above controls
    float exitXFactor;
    float fixedZoneHalfWidth;
    float fixedZoneExitX;
    // -------------------------------------
    Vec2 pos;

    // power ups
    bool sizeIncreased = false;
    bool hasLaserGun = false;
    float powerUpDuration_incrSize;      /* in seconds */
    float powerUpDuration_laserGun;      /* in seconds */
    std::chrono::steady_clock::time_point startTime_incrSize;
    std::chrono::steady_clock::time_point startTime_laserGun;
    RectF leftGun;
    RectF rightGun;

    float halfWidthOriginal = halfWidth;    /* for reducing size after power up duration */
};


