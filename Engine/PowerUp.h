#pragma once
#include "Graphics.h"
#include "RectF.h"
#include "Vec2.h"

enum ePowerUpType
{
    INCR_PADDLE_SIZE = 0,
    CANNON,
    EXTRA_LIFE
};

class PowerUp
{
public:
    PowerUp() = default;
    PowerUp( const float width_in, const float height_in, ePowerUpType type_in, 
             const float boost_time, const float level_bottom );    /* boost time in seconds */
    bool Update( const RectF& paddleRect, const float dt );         /* returns true, when hit paddle */
    void Draw( Graphics& gfx );
    void Activate( const Vec2& pos_in );
    ePowerUpType GetType() const;
    float GetBoostTime() const;
private:
    static constexpr float speed = 200;
    Vec2 pos;
    float width;
    float height;
    float boostTime;        /* how long does the effect last in seconds */
    ePowerUpType type;

    float levelBottom;      /* will be deactivated when this y-position is reached */
    bool activated = false;
};