#include "PowerUp.h"

PowerUp::PowerUp( const float width_in, const float height_in, ePowerUpType type_in, 
                  const float boost_time, const float level_bottom )
    :
    width( width_in ),
    height( height_in ),
    type( type_in ),
    boostTime( boost_time ),
    activated( false ),
    levelBottom( level_bottom )
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

    if( paddleRect.IsOverlappingWith( RectF( pos, width, height ) ) )
    {
        activated = false;
        return true;
    }

    return false;
}

void PowerUp::Draw( Graphics& gfx )
{
    if( !activated )
    {
        return;
    }

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
        gfx.DrawIsoRightTriUL( left, top, halfHeight, Colors::Black );
        gfx.DrawIsoRightTriBL( left, bottom - halfHeight, halfHeight, Colors::Black );
        gfx.DrawIsoRightTriUR( right - halfHeight, top, halfHeight, Colors::Black );
        gfx.DrawIsoRightTriBR( right - halfHeight, bottom - halfHeight, halfHeight, Colors::Black );
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
    else
    {

    }
}

void PowerUp::Activate( const Vec2& pos_in )
{
    if( activated )
    {
        return;
    }
    pos = pos_in;
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
