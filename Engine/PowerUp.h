#pragma once
#include "Graphics.h"
#include "RectF.h"

enum ePowerUpType
{
    INCR_PADDLE_SIZE = 0,
    CANNON,
    EXTRA_LIFE
};

class PowerUp
{
public:
    PowerUp( const RectF& rect_in, ePowerUpType type_in, const float boost_time ); /* boost time in seconds */
    void Update( const RectF& paddleRect, const float dt );
    void Draw( Graphics& gfx );
private:
    static constexpr float speed = 200;
    RectF rect;
    float boostTime;
    ePowerUpType type;
};