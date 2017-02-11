#pragma once
#include "RectF.h"
#include "Colors.h"
#include "Graphics.h"
#include "Ball.h"

class Brick
{
public:
    Brick() = default;
    Brick( const RectF& rect_in, Color color_in );
    void Draw( Graphics& gfx ) const;
    bool DoBallCollision( Ball& ball );
    bool IsDestroyed() const;
private:
    static constexpr float padding = 1.0f;
    RectF rect;
    Color color;
    bool destroyed = false;
};