#pragma once
#include "RectF.h"
#include "Graphics.h"

class LaserShot
{
public:
    LaserShot() = default;
    LaserShot( const Vec2& pos_in, float level_top );

    bool Update( float dt );    /* return true when left the game field */
    void Draw( Graphics& gfx ) const;
    //void DeActivate();
    //bool IsActivated() const;
    RectF GetRect() const;
private:
    static constexpr float speed = 700;
    static constexpr Color color = Colors::Yellow;
    Vec2 pos;   /* center of the shot */
    float halfWidth = 1;
    float halfHeight = 3;

    float levelTop;
    //bool activated = false;
};