#include "Brick.h"

Brick::Brick( const RectF& rect_in, Color color_in )
    :
    rect( rect_in ),
    color( color_in )
{
}

void Brick::Draw( Graphics& gfx ) const
{
    if( !destroyed )
    {
        gfx.DrawRect( rect.GetExpanded( -padding ), color );
    }
}

bool Brick::DoBallCollision( Ball& ball )
{
    if( !destroyed && rect.IsOverlappingWith( ball.GetRect() ) )
    {
        //if( ( ball.GetRect().bottom > rect.top + 1 && ball.GetRect().top < rect.bottom - 1 ) &&
        if( ball.GetPosition().x < rect.left || ball.GetPosition().x > rect.right )
        {
            ball.ReboundX();
        }
        else
        {
            ball.ReboundY();
        }        
        destroyed = true;
        return true;
    }
    return false;
}

bool Brick::IsDestroyed() const
{
    return destroyed;
}
