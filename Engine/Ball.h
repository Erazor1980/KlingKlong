#pragma once
#include "Vec2.h"
#include "Graphics.h"

class Ball
{
public:
    Ball() = default;
    Ball( const Vec2& pos_in, const Vec2& dir_in );
    void Draw( Graphics& gfx ) const;
    void Update( float dt, const Vec2& paddleCenter );
    /* return 0 = nothing, 1 = hit wall, 2 = hit bottom */
    int DoWallCollision( const RectF& walls );
    void ReboundX();
    void ReboundY();
    RectF GetRect() const;
    Vec2 GetDirection() const;
    Vec2 GetPosition() const;
    void SetDirection( const Vec2& dir_in );
    void Start();
    void Stop();
private:
    static constexpr float radius = 7.0f;
    float speed = 500;
    bool moving = false;    /* for starting game, or resetting ball after loosing a life */
    Vec2 pos;
    Vec2 dir;
};