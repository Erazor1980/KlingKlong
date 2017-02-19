#include "Laser.h"

LaserShot::LaserShot( const Vec2& pos_in, float level_top )
    :
    pos( pos_in ),
    levelTop( level_top )
{
}

bool LaserShot::Update( float dt )
{
    pos.y -= speed * dt;

    if( pos.y - halfHeight <= levelTop )
    {
        return true;
    }
    
    return false;
}

void LaserShot::Draw( Graphics& gfx ) const
{
    gfx.DrawRect( RectF::FromCenter( pos, halfWidth, halfHeight ), color );
}

RectF LaserShot::GetRect() const
{
    return RectF::FromCenter( pos, halfWidth, halfHeight );
}
