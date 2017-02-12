#pragma once
#include "Vec2.h"
#include "Graphics.h"

/* position, where ball hit the paddle */
enum eBouncePos
{
    LEFT = 0,
    MID_LEFT,
    CENTER,
    MID_RIGHT,
    RIGHT,
    INVALID
};

class Ball
{
public:
    Ball() = default;
    Ball( const Vec2& pos_in, const Vec2& vel_in );
    void Draw( Graphics& gfx ) const;
    void Update( float dt, const Vec2& paddleCenter );
    bool DoWallCollision( const RectF& walls );
    void ReboundX();
    void ReboundY( const eBouncePos bouncePos = CENTER );
    RectF GetRect() const;
    Vec2 GetVelocity() const;
    Vec2 GetPosition() const;
    void Start();
    void Stop();
private:
    static constexpr float radius = 7.0f;
    float speed = 500;
    bool moving = false;    /* for starting game, or resetting ball after loosing a life */
    Vec2 pos;
    Vec2 vel;
};