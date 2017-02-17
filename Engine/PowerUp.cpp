#include "PowerUp.h"

PowerUp::PowerUp( const float width_in, const float height_in, ePowerUpType type_in, 
                  const float boost_time, const float level_bottom, int seqImagesNumber )
    :
    width( width_in ),
    height( height_in ),
    type( type_in ),
    boostTime( boost_time ),
    activated( false ),
    levelBottom( level_bottom ),
    nSeqImages( seqImagesNumber )
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
        IdxSurfSeq++;
        if( IdxSurfSeq >= nSeqImages )
        {
            IdxSurfSeq = 0;
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

    gfx.DrawSpriteKeyFromSequence( ( int )pos.x, ( int )pos.y, surfSeq, surfSeq.GetPixel( 0, 0 ), IdxSurfSeq, nSeqImages );
    return;

    //TODO remove later, if sprites work!

    RectF rect( pos, width, height );

    const int left = int( rect.left );
    const int right = int( rect.right );
    const int top = int( rect.top );
    const int bottom = int( rect.bottom );
    const int width = right - left;
    const int halfHeight = ( bottom - top ) / 2;
    const int halfWidth = ( right - left ) / 2;
    if( INCR_PADDLE_SIZE == type )
    {
        gfx.DrawRect( rect, { 200, 20, 20 } );
        gfx.DrawIsoRightTriUL( left, top, halfHeight, Colors::White );
        gfx.DrawIsoRightTriBL( left, bottom - halfHeight, halfHeight, Colors::White );
        gfx.DrawIsoRightTriUR( right - halfHeight, top, halfHeight, Colors::White );
        gfx.DrawIsoRightTriBR( right - halfHeight, bottom - halfHeight, halfHeight, Colors::White );
        gfx.DrawRectBorder( rect, 1, Colors::White );
    }
    else if( EXTRA_LIFE == type )
    {
        gfx.DrawRect( rect, Colors::White );
        gfx.DrawRect( RectF::FromCenter( rect.GetCenter(), halfWidth / 2.0f - 1, halfHeight / 2.0f - 1 ), Colors::Red );
        gfx.DrawRect( RectF::FromCenter( rect.GetCenter(), halfHeight / 2.0f - 1, halfHeight - 1.0f ), Colors::Red );
        gfx.DrawRectBorder( rect, 2, Colors::Cyan );
    }
    else if( LASER_GUN == type )
    {
        gfx.DrawRect( rect, Colors::White );

        RectF r( Vec2( left + halfWidth / 4.0f, top + halfHeight - 1.0f ), ( float )halfWidth, halfHeight / 2.0f );
        gfx.DrawRect( r, Colors::Black );
        gfx.DrawCircle( ( int )pos.x + int( halfWidth * 1.5 ), ( int )pos.y + halfHeight, int( halfHeight / 1.5 ), Colors::Red );
    }
    else if( MULTI_BALL == type )
    {
        gfx.DrawRect( rect, Colors::White );
        gfx.DrawCircle( ( int )pos.x + int( halfWidth ), ( int )pos.y + halfHeight, int( halfHeight / 1.8 ), { 240, 90, 70 } );
        gfx.DrawCircle( ( int )pos.x + int( halfWidth * 1.5 ), ( int )pos.y + halfHeight, int( halfHeight / 1.8 ), { 240, 90, 70 } );
        gfx.DrawCircle( ( int )pos.x + int( halfWidth * 0.5 ), ( int )pos.y + halfHeight, int( halfHeight / 1.8 ), { 240, 90, 70 } );
    }
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
