#pragma once
#include "Vec2.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "Defines.h"
#include <cmath>
#include <chrono>

enum eBallState
{
    MOVING = 0, /* ball moving in game */
    WAITING,    /* starting game / after life loss -> direction can be changed */
    STICKING,   /* stuck to paddle -> moving with paddle */
    INACTIVE
};

class Ball
{
public:
    enum eBallWallColRes
    {
        NOTHING = 0,
        WALL_HIT,
        BOTTOM_HIT
    };
public:
    Ball() = default;
    Ball( const Vec2& pos_in, const Vec2& dir_in, int rowImagesSeq_in, int colImagesSeq_in );   /* sequence parameter for the super ball sequence! */
    void Draw( Graphics& gfx, const Surface& surfSeq ) const;
    void Update( float dt, const float paddleCenterX, const Keyboard& kbd );
    /* return 0 = nothing, 1 = hit wall, 2 = hit bottom */
   
    eBallWallColRes DoWallCollision( const RectF& walls, const RectF& paddle );
    void ReboundX( bool paddleBounce = false );
    void ReboundY( bool paddleBounce = false );
    RectF GetRect() const;
    Vec2 GetDirection() const;
    Vec2 GetPosition() const;
    eBallState GetState() const;
    void SetDirection( const Vec2& dir_in, bool padCooldown = false );
    void Start();
    void Stop();
    void StickToPaddle( const float paddleCenterX );
    bool HasPaddleCooldown() const;     /* to avoid weird paddle/ball things */
    void ActivateSuperBall( const float duration, float newRadius );
    void DeActivateSuperBall();
    bool IsSuperBall() const;
private:
    float radius = 7.0f;
#if EASY_MODE
    float speed = 270;
#else
    float speed  = 550;
#endif
    eBallState ballState = INACTIVE;
    float offsetToPaddleCenter = 0;     /* for STICKING state, to move with the paddle */
    Vec2 pos;
    Vec2 dir;

    bool paddleCooldown = false;

    // super ball
    bool superBallActive = false;
    int rowImagesSeq;
    int colImagesSeq;
    int idxSurfSeq = 0;
    std::chrono::steady_clock::time_point startTime;    /* to measure time between sequence images */

    float powerUpDuration_superBall;      /* in seconds */
    std::chrono::steady_clock::time_point startTime_superBall;
};