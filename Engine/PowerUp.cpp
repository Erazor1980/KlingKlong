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
    if( INCR_PADDLE_SIZE == type )
    {
        gfx.DrawRect( rect, { 200, 20, 20 } );
        gfx.DrawIsoRightTriUL( left, top, halfHeight, Colors::Black );
        gfx.DrawIsoRightTriBL( left, bottom - halfHeight, halfHeight, Colors::Black );
        gfx.DrawIsoRightTriUR( right - halfHeight, top, halfHeight, Colors::Black );
        gfx.DrawIsoRightTriBR( right - halfHeight, bottom - halfHeight, halfHeight, Colors::Black );
    }
    //TODO add other drawings, when new types implemented!
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

ePowerUpType PowerUp::GetType() const
{
    return type;
}

float PowerUp::GetBoostTime() const
{
    return boostTime;
}
