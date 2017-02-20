#include "Ball.h"
#include "SpriteCodex.h"

Ball::Ball( const Vec2& pos_in, const Vec2& dir_in, int rowImagesSeq_in, int colImagesSeq_in )
    :
    pos( pos_in ),
    ballState( WAITING ),
    rowImagesSeq( rowImagesSeq_in ),
    colImagesSeq( colImagesSeq_in )
{
    SetDirection( dir_in );
}

void Ball::Draw( Graphics& gfx, const Surface& surfSeq ) const
{
    // draw ball direction line when stopped
    if( ballState != MOVING )
    {
        Vec2 dirLine = pos + dir.GetNormalized();
#if EASY_MODE
        for( int i = 1; i <= 55; ++i )
#else
        for( int i = 1; i <= 15; ++i )
#endif
        {
            gfx.PutPixel( ( int )dirLine.x, ( int )dirLine.y, Colors::Gray );
            //gfx.DrawCircle( ( int )dirLine.x, ( int )dirLine.y, 2, Colors::Blue );
            dirLine += dir.GetNormalized() * 9;
        }
    }

    // draw ball
    if( superBallActive )
    {
        const float halfWidth = ( surfSeq.GetWidth() / rowImagesSeq ) / 2.0f;
        const float halfHeight = ( surfSeq.GetHeight() / colImagesSeq ) / 2.0f;
        gfx.DrawSpriteKeyFromSequence( ( int )( pos.x - halfWidth ), ( int )( pos.y - halfHeight ),
                                       surfSeq, surfSeq.GetPixel( 0, 0 ), idxSurfSeq, rowImagesSeq, colImagesSeq );
    }
    else
    {
        SpriteCodex::DrawBall( pos, gfx );
    }
}

void Ball::Update( float dt, const float paddleCenterX, const Keyboard& kbd )
{
    if( MOVING == ballState )
    {
        pos += dir.GetNormalized() * dt * speed;
        const std::chrono::duration<float> timeElapsed = std::chrono::steady_clock::now() - startTime;
        if( timeElapsed.count() > 0.055 )
        {
            idxSurfSeq++;
            if( idxSurfSeq >= rowImagesSeq * colImagesSeq )
            {
                idxSurfSeq = 0;
            }
            startTime = std::chrono::steady_clock::now();
        }
    }
    else if( WAITING == ballState )
    {        
        float angleChange = DEG2RAD( 0.3 );
        if( kbd.KeyIsPressed( VK_LEFT ) && atan2( dir.y, dir.x ) > DEG2RAD( -150 ) )
        {
            float rotatedX = dir.x * cos( -angleChange ) - dir.y * sin( -angleChange );
            float rotatedY = dir.x * sin( -angleChange ) + dir.y * cos( -angleChange );
            SetDirection( Vec2( rotatedX, rotatedY ) );
        }
        else if( kbd.KeyIsPressed( VK_RIGHT ) && atan2( dir.y, dir.x ) < DEG2RAD( -30 )  )
        {
            float rotatedX = dir.x * cos( angleChange ) - dir.y * sin( angleChange );
            float rotatedY = dir.x * sin( angleChange ) + dir.y * cos( angleChange );
            SetDirection( Vec2( rotatedX, rotatedY ) );
        }
    }
    else if( STICKING == ballState )
    {
        pos.x = paddleCenterX + offsetToPaddleCenter;
    }

    if( superBallActive )
    {
        const std::chrono::duration<float> timeElapsed = std::chrono::steady_clock::now() - startTime_superBall;
        if( timeElapsed.count() > powerUpDuration_superBall )
        {
            DeActivateSuperBall();
        }
    }
}

Ball::eBallWallColRes Ball::DoWallCollision( const RectF& walls, const RectF& paddle )
{
    bool padBounce = paddle.IsOverlappingWith( GetRect() );

    eBallWallColRes collisionResult = NOTHING;
    const RectF rect = GetRect();
    if( rect.left < walls.left )
    {
        pos.x += walls.left - rect.left;
        ReboundX( padBounce );
        collisionResult = WALL_HIT;
    }
    else if( rect.right > walls.right )
    {
        pos.x -= rect.right - walls.right;
        ReboundX( padBounce );
        collisionResult = WALL_HIT;
    }
    if( rect.top < walls.top )
    {
        pos.y += walls.top - rect.top;
        ReboundY( padBounce );
        collisionResult = WALL_HIT;
    }
    else if( rect.bottom > walls.bottom )
    {
        pos.y -=  rect.bottom - walls.bottom;
        ReboundY( padBounce );
        collisionResult = BOTTOM_HIT;
    }    

    return collisionResult;
}

void Ball::ReboundX( bool paddleBounce )
{
    dir.x = -dir.x;
    paddleCooldown = paddleBounce;
}

void Ball::ReboundY( bool paddleBounce )
{
    dir.y = -dir.y;
    paddleCooldown = paddleBounce;
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

eBallState Ball::GetState() const
{
    return ballState;
}

void Ball::SetDirection( const Vec2& dir_in, bool padCooldown )
{
    dir = dir_in;
    paddleCooldown = padCooldown;
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

bool Ball::HasPaddleCooldown() const
{
    return paddleCooldown;
}

void Ball::ActivateSuperBall( const float duration, float newRadius )
{
    superBallActive = true;

    if( WAITING == ballState )
    {
        pos.y -= newRadius - radius;
    }
    radius = newRadius;

    startTime_superBall = std::chrono::steady_clock::now();
    powerUpDuration_superBall = duration;
}

void Ball::DeActivateSuperBall()
{
    superBallActive = false;
    radius = 7;
}

bool Ball::IsSuperBall() const
{
    return superBallActive;
}
