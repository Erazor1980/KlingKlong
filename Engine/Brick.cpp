#include "Brick.h"
#include <assert.h>

Brick::Brick( const RectF& rect_in, Color color_in, int lifeToDestroy, bool isUndestroyable )
    :
    rect( rect_in ),
    color( color_in ),
    destroyed( false ),
    maxLife( lifeToDestroy ),
    life( maxLife ),
    undestroyable( isUndestroyable )
{
}

void Brick::Draw( Graphics& gfx ) const
{
    if( !destroyed )
    {
        gfx.DrawRect( rect.GetExpanded( -padding ), color );

        /* draw "damaged" brick */
        if( life < maxLife )
        {
            for( int y = ( int )rect.top; y < ( int )rect.bottom; ++y )
            {
                for( int x = ( int )rect.left; x < ( int )rect.right; ++x )
                {
                    if( x % 2 == 1 && y % 2 == 1 )
                    {
                        gfx.PutPixel( x, y, Colors::Black );
                    }
                }
            }
        }
    }
}

bool Brick::CheckBallCollision( const Ball & ball ) const
{
    return !destroyed && rect.IsOverlappingWith( ball.GetRect() );
}

bool Brick::ExecuteBallCollision( Ball& ball )
{
    assert( CheckBallCollision( ball ) );

    const Vec2 ballPos = ball.GetPosition();

    /* test for "inside" bouncer... */
    if( std::signbit( ball.GetVelocity().x ) == std::signbit( ( ballPos - GetCenter() ).x ) )
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

    if( !undestroyable )
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
