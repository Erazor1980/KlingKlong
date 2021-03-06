#pragma once
#include "Graphics.h"
#include "RectF.h"
#include "Vec2.h"
#include "Defines.h"
#include "Sound.h"
#include <chrono>

enum ePowerUpType
{
    INCR_PADDLE_SIZE = 0,
    EXTRA_LIFE,
    LASER_GUN,
    MULTI_BALL,
    SUPER_BALL,
    NUMBER_POWER_UPS
};

class PowerUp
{
public:
    PowerUp() = default;
    PowerUp( const Vec2& centerPos_in, const float width_in, const float height_in, ePowerUpType type_in, const float boost_time /* boost time in seconds */,
             const float level_bottom, int rowImagesSeq_in, int colImagesSeq_in, Sound* const sound_in, bool easy = false );
    bool Update( const RectF& paddleRect, const float dt, bool &paddleHit );         /* returns true, when hit paddle or the ground */
    void Draw( Graphics& gfx, const Surface& surfSeq ) const;
    ePowerUpType GetType() const;
    float GetBoostTime() const;
private:
    float speed = 250;
    Vec2 pos;
    float width;
    float height;
    float boostTime;        /* how long does the effect last in seconds */
    ePowerUpType type;
    Sound* sound;

    float levelBottom;      /* will be deactivated when this y-position is reached */
    //bool activated = false;

    int idxSurfSeq = 0;   /* which image of the sequence will be displayed */
    int rowImagesSeq;       /* amount of subimages per row in the sequence image*/
    int colImagesSeq;       /* amount of subimages per column in the sequence image */
    std::chrono::steady_clock::time_point startTime;    /* to measure time between sequence images */
};