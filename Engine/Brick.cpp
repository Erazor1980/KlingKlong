#include "Brick.h"
#include <assert.h>

Brick::Brick( const RectF& rect_in, Color color_in, eBrickType type_in, int lifeToDestroy )
    :
    rect( rect_in ),
    bev( color_in ),
    destroyed( false ),
    maxLife( lifeToDestroy ),
    life( maxLife ),
    type( type_in )
{
}

void Brick::Draw( Graphics& gfx ) const
{
    if( !destroyed )
    {
        bev.DrawBeveledBrick( rect.GetExpanded( -padding ), bevelSize, gfx );

        if( type == UNDESTROYABLE )
        {
            bev.DrawBeveledBrick( rect.GetExpanded( -padding * 6 ), bevelSize, gfx );
        }
        else if( type == SOLID )
        {
            if( life > 1 )
            {
                bev.DrawBeveledBrick( rect.GetExpanded( -padding * 12 ), bevelSize, gfx );
            }
        }        
    }
}

bool Brick::CheckBallCollision( const Ball& ball ) const
{
    return !destroyed && rect.IsOverlappingWith( ball.GetRect() );
}

bool Brick::ExecuteBallCollision( Ball& ball )
{
    assert( CheckBallCollision( ball ) );

    const Vec2 ballPos = ball.GetPosition();

    /* test for "inside" bouncer... */
    if( std::signbit( ball.GetDirection().x ) == std::signbit( ( ballPos - GetCenter() ).x ) )
    {
        ball.ReboundY();
    }
    else if( ballPos.x >= rect.left && ballPos.x <= rect.right )
    {
        ball.ReboundY();
    }
    else
    {
        ball.ReboundX();
    }

    if( type != UNDESTROYABLE )
    {
        life--;
        if( life <= 0 )
        {
            destroyed = true;
        }
    }

    return destroyed;
}

bool Brick::CheckLaserCollision( LaserShot& laser )
{
    if( destroyed || !rect.IsOverlappingWith( laser.GetRect() ) )
    {
        return false;
    }

    laser.DeActivate();

    if( type != UNDESTROYABLE )
    {
        life--;
        if( life <= 0 )
        {
            destroyed = true;
        }
    }

    return destroyed;
}

bool Brick::IsDestroyed() const
{
    return destroyed;
}

Vec2 Brick::GetCenter() const
{
    return rect.GetCenter();
}
