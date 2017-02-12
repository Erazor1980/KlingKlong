#include "Ball.h"
#include "SpriteCodex.h"

Ball::Ball( const Vec2& pos_in, const Vec2& vel_in )
    :
    pos( pos_in ),
    vel( vel_in )
{
}

void Ball::Draw( Graphics& gfx ) const
{
    // draw ball direction line when stopped
    if( !moving )
    {
        Vec2 dir = pos + vel.GetNormalized();
        for( int i = 1; i <= 10; ++i )
        {
            gfx.PutPixel( ( int )dir.x, ( int )dir.y, Colors::Gray );
            dir += vel.GetNormalized() * 7;
        }
    }

    // draw ball
    SpriteCodex::DrawBall( pos, gfx );
}

void Ball::Update( float dt, const Vec2& paddleCenter )
{
    if( moving )
    {
        pos += vel.GetNormalized() * dt * speed;
    }
    else
    {
        pos.x = paddleCenter.x;
    }
}

bool Ball::DoWallCollision( const RectF& walls )
{
    bool collided = false;
    const RectF rect = GetRect();
    if( rect.left < walls.left )
    {
        pos.x += walls.left - rect.left;
        ReboundX();
        collided = true;
    }
    else if( rect.right > walls.right )
    {
        pos.x -= rect.right - walls.right;
        ReboundX();
        collided = true;
    }
    if( rect.top < walls.top )
    {
        pos.y += walls.top - rect.top;
        ReboundY();
        collided = true;
    }
    else if( rect.bottom > walls.bottom )
    {
        pos.y -=  rect.bottom - walls.bottom;
        ReboundY();
        collided = true;
    }    

    return collided;
}

void Ball::ReboundX()
{
    vel.x = -vel.x;
}

void Ball::ReboundY( const eBouncePos bouncePos )
{
    switch( bouncePos )
    {
    case LEFT:
        vel = Vec2( -2, -1 ).GetNormalized();
        //moving = false;
        break;
    case MID_LEFT:
        vel = Vec2( -1, -1 ).GetNormalized();
        break;
    case CENTER:
        vel.y = -vel.y;
        break;
    case RIGHT:
        vel = Vec2( 2, -1 ).GetNormalized();
        //moving = false;
        break;
    case MID_RIGHT:
        vel = Vec2( 1, -1 ).GetNormalized();
        break;
    }
}

RectF Ball::GetRect() const
{
    return RectF::FromCenter( pos, radius, radius );
}

Vec2 Ball::GetVelocity() const
{
    return vel;
}

Vec2 Ball::GetPosition() const
{
    return pos;
}

void Ball::Start()
{
    moving = true;
}

void Ball::Stop()
{
    moving = false;
}
