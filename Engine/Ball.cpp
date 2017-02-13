#include "Ball.h"
#include "SpriteCodex.h"

Ball::Ball( const Vec2& pos_in, const Vec2& dir_in )
    :
    pos( pos_in )
{
    SetDirection( dir_in );
}

void Ball::Draw( Graphics& gfx ) const
{
    // draw ball direction line when stopped
    if(  ballState != MOVING )
    {
        Vec2 dirLine = pos + dir.GetNormalized();
        for( int i = 1; i <= 15; ++i )
        {
            gfx.PutPixel( ( int )dirLine.x, ( int )dirLine.y, Colors::Gray );
            dirLine += dir.GetNormalized() * 7;
        }
    }

    // draw ball
    SpriteCodex::DrawBall( pos, gfx );
}

void Ball::Update( float dt, const float paddleCenterX )
{
    if( MOVING == ballState )
    {
        pos += dir.GetNormalized() * dt * speed;
    }
    else if( WAITING == ballState )
    {
        //TODO
    }
    else if( STICKING == ballState )
    {
        pos.x = paddleCenterX + offsetToPaddleCenter;
    }
}

int Ball::DoWallCollision( const RectF& walls )
{
    int collisionResult = 0;
    const RectF rect = GetRect();
    if( rect.left < walls.left )
    {
        pos.x += walls.left - rect.left;
        ReboundX();
        collisionResult = 1;
    }
    else if( rect.right > walls.right )
    {
        pos.x -= rect.right - walls.right;
        ReboundX();
        collisionResult = 1;
    }
    if( rect.top < walls.top )
    {
        pos.y += walls.top - rect.top;
        ReboundY();
        collisionResult = 1;
    }
    else if( rect.bottom > walls.bottom )
    {
        pos.y -=  rect.bottom - walls.bottom;
        ReboundY();
        collisionResult = 2;
    }    

    return collisionResult;
}

void Ball::ReboundX()
{
    dir.x = -dir.x;
}

void Ball::ReboundY()
{
    dir.y = -dir.y;
}

RectF Ball::GetRect() const
{
    return RectF::FromCenter( pos, radius, radius );
}

Vec2 Ball::GetDirection() const
{
    return dir;
}

Vec2 Ball::GetPosition() const
{
    return pos;
}

void Ball::SetDirection( const Vec2& dir_in )
{
    dir = dir_in;
}

void Ball::Start()
{
    ballState = MOVING;
}

void Ball::Stop()
{
    ballState = WAITING;
}

void Ball::StickToPaddle( const float paddleCenterX )
{
    offsetToPaddleCenter = pos.x - paddleCenterX;
    ballState = STICKING;
}
