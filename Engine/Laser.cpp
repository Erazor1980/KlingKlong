#include "Laser.h"

LaserShot::LaserShot( const Vec2& pos_in, float level_top )
    :
    pos( pos_in ),
    levelTop( level_top ),
    activated( true )
{
}

void LaserShot::Update( float dt )
{
    if( !activated )
    {
        return;
    }

    pos.y -= speed * dt;

    if( pos.y - halfHeight <= levelTop )
    {
        activated = false;
        return;
    }
}

void LaserShot::Draw( Graphics& gfx )
{
    if( !activated )
    {
        return;
    }

    gfx.DrawRect( RectF::FromCenter( pos, halfWidth, halfHeight ), color );
}

void LaserShot::DeActivate()
{
    activated = false;
}

bool LaserShot::IsActivated() const
{
    return activated;
}

RectF LaserShot::GetRect() const
{
    return RectF::FromCenter( pos, halfWidth, halfHeight );
}
