#include "Paddle.h"

Paddle::Paddle( const Vec2& pos_in )
    :
    pos( pos_in ),
    exitXFactor( maximumExitRatio / halfWidth ),
    fixedZoneHalfWidth( halfWidth * fixedZoneWidthRatio ),
    fixedZoneExitX( fixedZoneHalfWidth * exitXFactor ),
    hasLaserGun( false ),
    sizeIncreased( false )
{
    CalcLaserPositions();
}

void Paddle::Draw( Graphics& gfx ) const
{
    //RectF rect = GetRect();
    //gfx.DrawRect( rect, wingColor );
    //rect.left += wingWidth;
    //rect.right -= wingWidth;
    //gfx.DrawRect( rect, color );

    if( hasLaserGun )
    {
        gfx.DrawRect( leftGun, Colors::Gray );
        gfx.DrawRect( rightGun, Colors::Gray );
    }

    if( sizeIncreased )
    {
        gfx.DrawSpriteKey( ( int )pos.x - surfBig.GetWidth() / 2, ( int )pos.y - surfBig.GetHeight() / 2, surfBig, surfBig.GetPixel( 0, 0 ) );
    }
    else
    {
        gfx.DrawSpriteKey( ( int )pos.x - surf.GetWidth() / 2, ( int )pos.y - surf.GetHeight() / 2, surf, surf.GetPixel( 0, 0 ) );
    }

    //TODO test, remove later!
    //gfx.DrawRectBorder( GetRect(), 1, Colors::White );
}

void Paddle::DrawAsLifesRemaining( Graphics &gfx, const int lifesRemaining, const Vec2& pos, const float sizeRatio ) const
{
    /* calc size of life paddles to draw */
    const float w = ( halfWidthOriginal * 2 ) * sizeRatio;
    const float h = ( halfHeight * 2 ) * sizeRatio;
    const float wingW = wingWidth * sizeRatio;

    for( int i = 0; i < lifesRemaining; ++i )
    {
        RectF rect( pos + Vec2( i * 1.5f * w, 0 ), w, h );
        gfx.DrawRect( rect, wingColor );
        rect.left += wingW;
        rect.right -= wingW;
        gfx.DrawRect( rect, color );
    }
}

bool Paddle::DoBallCollision( Ball& ball )
{
    if( !ball.HasPaddleCooldown() && ball.GetState() == MOVING )
    {
        const RectF rect = GetRect();
        if( rect.IsOverlappingWith( ball.GetRect() ) )
        {
            const Vec2 ballPos = ball.GetPosition();
            if( std::signbit( ball.GetDirection().x ) == std::signbit( ( ballPos - pos ).x )
                || ( ballPos.x >= rect.left && ballPos.x <= rect.right ) )
            {
                Vec2 dir;
                const float xDifference = ballPos.x - pos.x;
                if( std::abs( xDifference ) < fixedZoneHalfWidth )
                {
                    if( xDifference < 0.0f )
                    {
                        dir = Vec2( -fixedZoneExitX, -1.0f );
                    }
                    else
                    {
                        dir = Vec2( fixedZoneExitX, -1.0f );
                    }
                }
                else
                {
                    dir = Vec2( xDifference * exitXFactor, -1.0f );
                }
                ball.SetDirection( dir, true );
            }
            else
            {
                ball.ReboundX( true );
            }
            return true;
        }
    }
    return false;
}

void Paddle::DoWallCollision( const RectF& walls )
{
    const RectF rect = GetRect();
    if( rect.left < walls.left )
    {
        pos.x += walls.left - rect.left;
    }
    else if( rect.right > walls.right )
    {
        pos.x -= rect.right - walls.right;
    }
}

void Paddle::Update( const Keyboard& kbd, float dt )
{
    if( kbd.KeyIsPressed( VK_LEFT ) )
    {
        pos.x -= speed * dt;
        CalcLaserPositions();
    }
    if( kbd.KeyIsPressed( VK_RIGHT ) )
    {
        pos.x += speed * dt;
        CalcLaserPositions();
    }

    if( sizeIncreased )
    {
        const std::chrono::duration<float> timeElapsed = std::chrono::steady_clock::now() - startTime_incrSize;
        if( timeElapsed.count() > powerUpDuration_incrSize )
        {
            halfWidth = halfWidthOriginal;
            sizeIncreased = false;
            CalcLaserPositions();
        }
    }

    if( hasLaserGun )
    {
        const std::chrono::duration<float> timeElapsed = std::chrono::steady_clock::now() - startTime_laserGun;
        if( timeElapsed.count() > powerUpDuration_laserGun )
        {
            hasLaserGun = false;
        }
    }
}

RectF Paddle::GetRect() const
{
    return RectF::FromCenter( pos, halfWidth, halfHeight );
}

RectF Paddle::GetLeftGunPosition() const
{
    return leftGun;
}

RectF Paddle::GetRightGunPosition() const
{
    return rightGun;
}

void Paddle::IncreaseSize( const float duration )
{
    startTime_incrSize = std::chrono::steady_clock::now();

    if( !sizeIncreased )
    {
        halfWidth = surfBig.GetWidth() / 2.0f;
        powerUpDuration_incrSize = duration;
        sizeIncreased = true;
        CalcLaserPositions();
    }
}

void Paddle::AddLaserGun( const float duration )
{
    startTime_laserGun = std::chrono::steady_clock::now();

    if( !hasLaserGun )
    {
        powerUpDuration_laserGun = duration;
        hasLaserGun = true;
        CalcLaserPositions();
    }
}

bool Paddle::HasLaserGun() const
{
    return hasLaserGun;
}

void Paddle::CalcLaserPositions()
{
    RectF rect = GetRect();
    leftGun = RectF( Vec2( rect.left + wingWidth + 1, rect.top - wingWidth / 2 ), wingWidth / 2, wingWidth / 2 );
    rightGun = RectF( Vec2( rect.right - wingWidth * 1.5f - 1, rect.top - wingWidth / 2 ), wingWidth / 2, wingWidth / 2 );
}
