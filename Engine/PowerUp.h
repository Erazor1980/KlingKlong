#pragma once
#include "Graphics.h"
#include "RectF.h"
#include "Vec2.h"
#include <chrono>

enum ePowerUpType
{
    INCR_PADDLE_SIZE = 0,
    EXTRA_LIFE,
    LASER_GUN,
    MULTI_BALL
};

class PowerUp
{
public:
    PowerUp() = default;
    PowerUp( const float width_in, const float height_in, ePowerUpType type_in, const float boost_time, /* boost time in seconds */
             const float level_bottom, int seqImagesNumber );    
    bool Update( const RectF& paddleRect, const float dt );         /* returns true, when hit paddle */
    void Draw( Graphics& gfx, const Surface& surfSeq );
    void Activate( const Vec2& pos_in, float brickWidth );  /* passing brickWidth to spawn centered (because width != brickWidth!) */
    void DeActivate();
    ePowerUpType GetType() const;
    float GetBoostTime() const;
    bool IsActivated() const;
private:
    static constexpr float speed = 300;
    Vec2 pos;
    float width;
    float height;
    float boostTime;        /* how long does the effect last in seconds */
    ePowerUpType type;

    float levelBottom;      /* will be deactivated when this y-position is reached */
    bool activated = false;

    int IdxSurfSeq = 0;   /* which image of the sequence will be displayed */
    int nSeqImages;       /* amount of subimages the sequence consists of */
    std::chrono::steady_clock::time_point startTime;    /* to measure time between sequence images */
};