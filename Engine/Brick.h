#pragma once
#include "RectF.h"
#include "Colors.h"
#include "Graphics.h"
#include "Ball.h"

class Brick
{
public:
    Brick() = default;
    Brick( const RectF& rect_in, Color color_in, bool isUndestroyable = false );
    void Draw( Graphics& gfx ) const;
    bool CheckBallCollision( const Ball& ball ) const;
    bool ExecuteBallCollision( Ball& ball );    /* returns true, if brick has been destroyed */
    bool IsDestroyed() const;
    Vec2 GetCenter() const;
private:
    static constexpr float padding = 1.0f;
    RectF rect;
    Color color;
    bool destroyed = true;
    bool undestroyable = false;
};