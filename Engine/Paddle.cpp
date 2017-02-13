#include "Paddle.h"

Paddle::Paddle( const Vec2& pos_in, float halfWidth_in, float halfHeight_in )
    :
    pos( pos_in ),
    halfWidth( halfWidth_in ),
    halfHeight( halfHeight_in )
{
}

void Paddle::Draw( Graphics& gfx ) const
{
    RectF rect = GetRect();
    gfx.DrawRect( rect, wingColor );
    rect.left += wingWidth;
    rect.right -= wingWidth;
    gfx.DrawRect( rect, color );

    // draw 2 lines for "mid_right/left"
    rect.left = GetRect().left + 2 * wingWidth;
    rect.right = GetRect().left + 2 * wingWidth + 1;
    gfx.DrawRect( rect, wingColor );
    rect.left = GetRect().right - 2 * wingWidth - 1;
    rect.right = GetRect().right - 2 * wingWidth;
    gfx.DrawRect( rect, wingColor );

#if EXTRA_DEBUG_STUFF
    if( bounce < DEFAULT )
    {
        switch( bounce )
        {
        case LEFT:
            gfx.DrawCircle( GetRect().left + 5, GetRect().top - 5, 3, Colors::Blue );
            break;
        case MID_LEFT:
            gfx.DrawCircle( GetRect().left + 5 + wingWidth, GetRect().top - 5, 3, Colors::Blue );
            break;
        case CENTER:
            gfx.DrawCircle( pos.x, GetRect().top - 5, 3, Colors::Blue );
            break;
        case RIGHT:
            gfx.DrawCircle( GetRect().right - 5, GetRect().top - 5, 3, Colors::Blue );
            break;
        case MID_RIGHT:
            gfx.DrawCircle( GetRect().right - 5 - wingWidth, GetRect().top - 5, 3, Colors::Blue );
            break;
        }
    }
#endif
}

bool Paddle::DoBallCollision( Ball& ball )
{
    if( !isCooldwon )
    {
        const RectF rect = GetRect();
        if( rect.IsOverlappingWith( ball.GetRect() ) )
        {
            const Vec2 ballPos = ball.GetPosition();

            if( ( std::signbit( ball.GetDirection().x ) == std::signbit( ( ballPos - pos ).x ) )
                || ( ballPos.x >= rect.left && ballPos.x <= rect.right ) )
            {
                const float xDiff = ballPos.x - pos.x;
                const Vec2 dir( xDiff * exitXFactor, -1.0f );
                ball.SetDirection( dir );
                //ball.setdi
            }
            else
            {
                ball.ReboundX();
            }
            isCooldwon = true;
            return true;
        }
    }
    return false;
}

#if 0 // old
bool Paddle::DoBallCollision( Ball& ball )
{
    if( !isCooldwon )
    {
        const RectF rect = GetRect();
        if( rect.IsOverlappingWith( ball.GetRect() ) )
        {
            const Vec2 ballPos = ball.GetPosition();

            if( std::signbit( ball.GetVelocity().x ) == std::signbit( ( ballPos - pos ).x ) )
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
            isCooldwon = true;
            return true;
        }
    }
    return false;
}
#endif
void Paddle::DoWallCollision( const RectF& walls )
{
    RectF rect = GetRect();
    if( rect.left < walls.left )
    {
        pos.x += walls.left - rect.left;
    }
    if( rect.right > walls.right )
    {
        pos.x -= rect.right - walls.right;
    }
}

void Paddle::Update( Keyboard& kbd, float dt )
{
    if( kbd.KeyIsPressed( VK_LEFT ) )
    {
        pos.x -= speed * dt;
    }
    if( kbd.KeyIsPressed( VK_RIGHT ) )
    {
        pos.x += speed * dt;
    }
}

RectF Paddle::GetRect() const
{
    return RectF::FromCenter( pos, halfWidth, halfHeight );
}

void Paddle::ResetCooldown()
{
    isCooldwon = false;
}
