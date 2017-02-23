#include "PowerUp.h"

PowerUp::PowerUp( const Vec2& centerPos_in, const float width_in, const float height_in, ePowerUpType type_in, const float boost_time /* boost time in seconds */,
                  const float level_bottom, int rowImagesSeq_in, int colImagesSeq_in, Sound* const sound_in, bool easy )
    :    
    width( width_in ),
    height( height_in ),
    type( type_in ),
    boostTime( boost_time ),
    //activated( false ),
    levelBottom( level_bottom ),
    rowImagesSeq( rowImagesSeq_in ),
    colImagesSeq( colImagesSeq_in ),
    sound( sound_in )
{
    pos = centerPos_in;
    pos.x = centerPos_in.x - width / 2.0f;

    if( easy )
    {
        speed = 150;
    }
}

bool PowerUp::Update( const RectF& paddleRect, const float dt, bool &paddleHit )
{
    pos.y += speed * dt;

    if( pos.y + height > levelBottom )
    {
        paddleHit = false;
        return true;
    }

    const std::chrono::duration<float> timeElapsed = std::chrono::steady_clock::now() - startTime;
    float ms = 0.05f;
    if( LASER_GUN == type )
    {
        ms = 0.10f;
    }
    if( timeElapsed.count() > ms )
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
        if( sound )
        {
            sound->Play( 1, 1.5f );
        }
        
        paddleHit = true;
        return true;
    }

    return false;
}

void PowerUp::Draw( Graphics& gfx, const Surface& surfSeq ) const
{
    //if( !activated )
    //{
    //    return;
    //}

    gfx.DrawSpriteKeyFromSequence( ( int )pos.x, ( int )pos.y, surfSeq, surfSeq.GetPixel( 0, 0 ), idxSurfSeq, rowImagesSeq, colImagesSeq );
    return;
}

//void PowerUp::Activate( const Vec2& pos_in, float brickWidth )
//{
//    if( activated )
//    {
//        return;
//    }
//    pos = pos_in;
//    pos.x += ( brickWidth - width ) / 2;
//    activated = true;
//}

//void PowerUp::DeActivate()
//{
//    activated = false;
//    idxSurfSeq = 0;
//}

ePowerUpType PowerUp::GetType() const
{
    return type;
}

float PowerUp::GetBoostTime() const
{
    return boostTime;
}

//bool PowerUp::IsActivated() const
//{
//    return activated;
//}
