#include "Enemy.h"

Enemy::Enemy( const Vec2& pos_in, const float width_in, const float height_in, const RectF& walls_in, int rowImagesSeq_in, int colImagesSeq_in )
    :
    walls( walls_in ),
    rowImagesSeq( rowImagesSeq_in ),
    colImagesSeq( colImagesSeq_in ),
    width( width_in ),
    height( height_in ),
    pos( pos_in )
{
    do
    {
        dir = Vec2( rand() % 100 - 50 + 10.0f, rand() % 100 - 50 + 10.0f );
    } while( abs( dir.x ) < 10 && abs( dir.y ) < 10 );
}

void Enemy::Draw( Graphics& gfx, const Surface& surfSeq ) const
{
    gfx.DrawSpriteKeyFromSequence( ( int )pos.x, ( int )pos.y, surfSeq, surfSeq.GetPixel( 0, 0 ), idxSurfSeq, rowImagesSeq, colImagesSeq );
}

void Enemy::Update( float dt )
{
    pos += dir.GetNormalized() * dt * speed;

    DoWallCollision();

    const std::chrono::duration<float> timeElapsed = std::chrono::steady_clock::now() - startTime;
    if( timeElapsed.count() > 0.025 )
    {
        idxSurfSeq++;
        if( idxSurfSeq >= rowImagesSeq * colImagesSeq )
        {
            idxSurfSeq = 0;
        }
        startTime = std::chrono::steady_clock::now();
    }
}

bool Enemy::CheckForCollision( const RectF& otherRect )
{
    if( GetRect().IsOverlappingWith( otherRect ) )
    {
        return true;
    }
    return false;
}

RectF Enemy::GetRect() const
{
    return RectF( pos, width, height );
}

Vec2 Enemy::GetPos() const
{
    return pos;
}

void Enemy::DoWallCollision()
{
    const RectF rect = GetRect();
    if( rect.left < walls.left )
    {
        pos.x += walls.left - rect.left;
        dir.x = -dir.x;
    }
    else if( rect.right > walls.right )
    {
        pos.x -= rect.right - walls.right;
        dir.x = -dir.x;
    }
    if( rect.top < walls.top )
    {
        pos.y += walls.top - rect.top;
        dir.y = -dir.y;
    }
    else if( rect.bottom > walls.bottom )
    {
        pos.y -=  rect.bottom - walls.bottom;
        dir.y = -dir.y;
    }
}
