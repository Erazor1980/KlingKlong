#pragma once
#include "RectF.h"
#include "Colors.h"
#include "Graphics.h"
#include "Ball.h"
#include "Beveler.h"

enum eBrickType
{
    STANDARD = 0,   /* standart 1 life brick*/
    SOLID,          /* solid brick, with 2+ life */
    UNDESTROYABLE   /* cannot be destroyed (maybe later with other balls!) */
};

class Brick
{
public:
    Brick() = default;
    Brick( const RectF& rect_in, Color color_in, eBrickType type_in = STANDARD, int lifeToDestroy = 1 );
    void Draw( Graphics& gfx ) const;
    bool CheckBallCollision( const Ball& ball ) const;
    bool ExecuteBallCollision( Ball& ball );    /* returns true, if brick has been destroyed */
    bool IsDestroyed() const;
    Vec2 GetCenter() const;
private:
    static constexpr float padding = 0.5f;
    static constexpr int bevelSize = 3;
    Beveler bev;
    RectF rect;
    bool destroyed = true;
    eBrickType type = STANDARD;
    int maxLife = 1;    /* so many hits needed to get destroyed */
    int life = 1;   
};