#pragma once
#include "RectF.h"
#include "Graphics.h"

class LaserShot
{
public:
    LaserShot() = default;
    LaserShot( const Vec2& pos_in, float level_top );

    void Update( float dt );
    void Draw( Graphics& gfx );
    void DeActivate();
    bool IsActivated() const;
    RectF GetRect() const;
private:
    static constexpr float speed = 700;
    static constexpr Color color = Colors::Yellow;
    Vec2 pos;   /* center of the shot */
    float halfWidth = 1;
    float halfHeight = 3;

    float levelTop;
    bool activated = false;
};