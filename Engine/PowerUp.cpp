#include "PowerUp.h"

PowerUp::PowerUp( const RectF& rect_in, ePowerUpType type_in, const float boost_time )
    :
    rect( rect_in ),
    type( type_in ),
    boostTime( boost_time )
{
}

void PowerUp::Update( const RectF& paddleRect, const float dt )
{
    rect.top += speed * dt;
    rect.bottom += speed * dt;
}

void PowerUp::Draw( Graphics& gfx )
{
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
