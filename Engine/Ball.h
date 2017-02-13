#pragma once
#include "Vec2.h"
#include "Graphics.h"
#include "Keyboard.h"
#include <cmath>

#define M_PI       3.14159265358979323846

// Converts degrees to radians.
#define DEG2RAD( angleDegrees ) ( float )( angleDegrees * M_PI / 180.0 )

// Converts radians to degrees.
#define RAD2DEG( angleRadians ) ( float )( angleRadians * 180.0 / M_PI )

enum eBallState
{
    MOVING = 0, /* ball moving in game */
    WAITING,    /* starting game / after life loss -> direction can be changed */
    STICKING    /* stuck to paddle -> moving with paddle */
};

class Ball
{
public:
    Ball() = default;
    Ball( const Vec2& pos_in, const Vec2& dir_in );
    void Draw( Graphics& gfx ) const;
    void Update( float dt, const float paddleCenterX, const Keyboard& kbd );
    /* return 0 = nothing, 1 = hit wall, 2 = hit bottom */
    int DoWallCollision( const RectF& walls );
    void ReboundX();
    void ReboundY();
    RectF GetRect() const;
    Vec2 GetDirection() const;
    Vec2 GetPosition() const;
    eBallState GetState() const;
    void SetDirection( const Vec2& dir_in );
    void Start();
    void Stop();
    void StickToPaddle( const float paddleCenterX );
private:
    static constexpr float radius = 7.0f;
    float speed = 500;
    eBallState ballState = WAITING;
    float offsetToPaddleCenter = 0;     /* for STICKING state, to move with the paddle */
    Vec2 pos;
    Vec2 dir;
};