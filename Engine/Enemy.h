#pragma once
#include "Vec2.h"
#include "Graphics.h"
#include <chrono>

class Enemy
{
public:
    Enemy() = default;
    Enemy( const float width_in, const float height_in, const RectF& walls_in, int rowImagesSeq_in, int colImagesSeq_in );
    void Draw( Graphics& gfx, const Surface& surfSeq ) const;
    void Update( float dt );
    bool CheckForCollision( const RectF& ballRect );
    RectF GetRect() const;
    Vec2 GetPos() const;
    void Activate( const Vec2& pos_in );
private:
    void DoWallCollision();

    Vec2 pos;
    Vec2 dir;
    float width;
    float height;
    float speed = 200;
    bool activated = false;

    RectF walls;

    int idxSurfSeq = 0;   /* which image of the sequence will be displayed */
    int rowImagesSeq;       /* amount of subimages per row in the sequence image*/
    int colImagesSeq;       /* amount of subimages per column in the sequence image */
    std::chrono::steady_clock::time_point startTime;    /* to measure time between sequence images */
};