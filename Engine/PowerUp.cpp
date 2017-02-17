#include "PowerUp.h"

PowerUp::PowerUp( const float width_in, const float height_in, ePowerUpType type_in, 
                  const float boost_time, const float level_bottom, int rowImagesSeq_in, int colImagesSeq_in )
    :
    width( width_in ),
    height( height_in ),
    type( type_in ),
    boostTime( boost_time ),
    activated( false ),
    levelBottom( level_bottom ),
    rowImagesSeq( rowImagesSeq_in ),
    colImagesSeq( colImagesSeq_in )
{
}

bool PowerUp::Update( const RectF& paddleRect, const float dt )
{
    if( !activated )
    {
        return false;
    }

    pos.y += speed * dt;

    if( pos.y + height > levelBottom )
    {
        activated = false;
        return false;
    }

    const std::chrono::duration<float> timeElapsed = std::chrono::steady_clock::now() - startTime;
    if( timeElapsed.count() > 0.05 )
    {
        idxSurfSeq++;
        if( idxSurfSeq >= rowImagesSeq * colImagesSeq )
        {
            idxSurfSeq = 0;
        }
        startTime = std::chrono::steady_clock::now();
    }


    if( paddleRect.IsOverlappingWith( RectF( pos, width, height ) ) )
    {
        activated = false;
        return true;
    }

    return false;
}

void PowerUp::Draw( Graphics& gfx, const Surface& surfSeq )
{
    if( !activated )
    {
        return;
    }

    gfx.DrawSpriteKeyFromSequence( ( int )pos.x, ( int )pos.y, surfSeq, surfSeq.GetPixel( 0, 0 ), idxSurfSeq, rowImagesSeq, colImagesSeq );
    return;
}

void PowerUp::Activate( const Vec2& pos_in, float brickWidth )
{
    if( activated )
    {
        return;
    }
    pos = pos_in;
    pos.x += ( brickWidth - width ) / 2;
    activated = true;
}

void PowerUp::DeActivate()
{
    activated = false;
    idxSurfSeq = 0;
}

ePowerUpType PowerUp::GetType() const
{
    return type;
}

float PowerUp::GetBoostTime() const
{
    return boostTime;
}

bool PowerUp::IsActivated() const
{
    return activated;
}
