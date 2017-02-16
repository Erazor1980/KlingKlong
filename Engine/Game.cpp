/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"
#include <ctime>

Game::Game( MainWindow& wnd )
    :
    wnd( wnd ),
    gfx( wnd ),
    soundPad( L"Sounds\\arkpad.wav" ),
    soundBrick( L"Sounds\\arkbrick.wav" ),
    soundLifeLoss( L"Sounds\\fart1.wav" ),
    soundGameOver( L"Sounds\\gameover.wav" ),
    soundVictory( L"Sounds\\victory.wav" ),
    soundLaserShot( L"Sounds\\laserShot.wav" ),
    walls( RectF::FromCenter( Graphics::GetScreenRect().GetCenter(), fieldWidth / 2.0f, fieldHeight / 2.0f ), wallThickness, wallColor )
{
    powerUps[ 0 ] = PowerUp( brickWidth, brickHeight, INCR_PADDLE_SIZE, 5, walls.GetInnerBounds().bottom );
    powerUps[ 1 ] = PowerUp( brickWidth, brickHeight, EXTRA_LIFE, 0, walls.GetInnerBounds().bottom );
    powerUps[ 2 ] = PowerUp( brickWidth, brickHeight, LASER_GUN, 4, walls.GetInnerBounds().bottom );

    powerUpSounds[ 0 ] = Sound( L"Sounds\\grow.wav" );
    powerUpSounds[ 1 ] = Sound( L"Sounds\\extraLife.wav" );

    ResetGame();
}

void Game::ResetGame()
{
    // seed rand()
    std::srand( ( unsigned int )std::time( 0 ) );

    // reset paddle
    ResetPaddle();

    // reset ball
    ResetBall();
    
    // reset bricks
    CreateNextLevel();

    // reset life
    lifes = MAX_LIFES;

    // reset power ups
    ResetPowerUps();

    // reset shots
    ResetShots();

    /////////////////
    //// TESTING ////
    /////////////////
#if 0
    //powerUps[ 0 ].Activate( Vec2( walls.GetInnerBounds().GetCenter().x, 400 ) );
    powerUps[ 2 ].Activate( Vec2( walls.GetInnerBounds().GetCenter().x, 300 ) );
    //laserShots[ 0 ] = LaserShot( Vec2( 400, 500 ), walls.GetInnerBounds().top );
#endif
}

void Game::ResetBall()
{
    ball = Ball( Vec2( pad.GetRect().GetCenter().x, pad.GetRect().top - 7 ) , Vec2( -0.2, -1 ) );
    ball.Stop();
}

void Game::ResetPaddle()
{
    pad = Paddle( Vec2( gfx.ScreenWidth / 2, gfx.ScreenHeight - 80 ), 70, 15 );
}

void Game::ResetPowerUps()
{
    for( int i = 0; i < nPowerUps; ++i )
    {
        powerUps[ i ].DeActivate();
    }
}

void Game::ResetShots()
{
    for( int i = 0; i < nMaxLaserShots; ++i )
    {
        laserShots[ i ] = LaserShot();
    }
    startedShooting = false;
}

void Game::ApplyPowerUp( const PowerUp& pu )
{
    switch( pu.GetType() )
    {
    case INCR_PADDLE_SIZE:
        pad.IncreaseSize( pu.GetBoostTime() );
        break;
    case EXTRA_LIFE:
        if( lifes < MAX_LIFES )
        {
            lifes++;
        }
        break;
    case LASER_GUN:
        pad.AddLaserGun( pu.GetBoostTime() );
        startTime_shot = std::chrono::steady_clock::now();
        break;
    default:
        break;
    }
}

void Game::Shoot()
{
    if( !pad.HasLaserGun() )
    {
        return;
    }
    
    const std::chrono::duration<float> timeElapsed = std::chrono::steady_clock::now() - startTime_shot;
    if( timeElapsed.count() > timeBetweenShots || !startedShooting )
    {
        soundLaserShot.Play();
        laserShots[ shotIdx ] = LaserShot( pad.GetRightGunPosition().GetCenter(), walls.GetInnerBounds().top );
        shotIdx++;
        if( shotIdx == nMaxLaserShots )
        {
            shotIdx = 0;
        }
        laserShots[ shotIdx ] = LaserShot( pad.GetLeftGunPosition().GetCenter(), walls.GetInnerBounds().top );
        shotIdx++;
        if( shotIdx == nMaxLaserShots )
        {
            shotIdx = 0;
        }
        startTime_shot = std::chrono::steady_clock::now();

        startedShooting = true;
    }
}

void Game::CreatePowerUp( int curColIdx )
{
#if EASY_MODE
    if( rand() % 3 == 1 )
    {
        powerUps[ 0 ].Activate( bricks[ curColIdx ].GetCenter() - Vec2( brickWidth / 2, 0 ) );
    }
    else if( lifes < MAX_LIFES && rand() % 3 == 1 )
    {
        powerUps[ 1 ].Activate( bricks[ curColIdx ].GetCenter() - Vec2( brickWidth / 2, 0 ) );
    }
#else
    if( rand() % 6 == 1 )  /* increased size */
    {
        powerUps[ 0 ].Activate( bricks[ curColIdx ].GetCenter() - Vec2( brickWidth / 2, 0 ) );
    }
    else if( lifes < MAX_LIFES && rand() % 6 == 1 )    /* extra life */
    {
        powerUps[ 1 ].Activate( bricks[ curColIdx ].GetCenter() - Vec2( brickWidth / 2, 0 ) );
    }
    else if( rand() % 6 == 1 )  /* laser gun */
    {
        powerUps[ 2 ].Activate( bricks[ curColIdx ].GetCenter() - Vec2( brickWidth / 2, 0 ) );
    }
#endif
}

void Game::CreateNextLevel()
{
    if( level > 2 )
    {
        level = 0;
    }

    /* clear all bricks first */
    for( int i = 0; i < nBricks; ++i )
    {
        bricks[ i ] = Brick();
    }

    const Vec2 topLeft( walls.GetInnerBounds().left + distWallBricks, walls.GetInnerBounds().top + 50 );
    if( 0 == level )
    {
        nBricksLeft = 0;
        for( int y = 0; y < nBricksDown; ++y )
        {
            const Color c = brickColors[ y ];
            for( int x = 0; x < nBricksAcross; ++x )
            {
                if( 0 == x || nBricksAcross - 1 == x )
                {
                    bricks[ y * nBricksAcross + x ] = Brick( RectF( topLeft + Vec2( x * brickWidth, y * brickHeight ),
                                                                    brickWidth, brickHeight ), Colors::LightGray, UNDESTROYABLE );
                }
                else
                {
                    if( 0 == y && x % 2 )
                    {
                        bricks[ y * nBricksAcross + x ] = Brick( RectF( topLeft + Vec2( x * brickWidth, y * brickHeight ),
                                                                        brickWidth, brickHeight ), Colors::MakeRGB( 205, 102, 29 ), SOLID, 2 );
                        nBricksLeft++;
                    }
                    else
                    {
                        bricks[ y * nBricksAcross + x ] = Brick( RectF( topLeft + Vec2( x * brickWidth, y * brickHeight ), brickWidth, brickHeight ), c );
                        nBricksLeft++;
                    }
                }
            }
        }
    }
    else if( 1 == level )
    {
        nBricksLeft = 0;
        for( int y = 0; y < nBricksDown; ++y )
        {
            const Color c = brickColors[ 3 - y ];
            for( int x = 0; x < nBricksAcross; ++x )
            {
                if( 3 == y && ( x < 3 || x > 9 ) )
                {
                    bricks[ y * nBricksAcross + x ] = Brick( RectF( topLeft + Vec2( x * brickWidth, y * brickHeight ),
                                                                    brickWidth, brickHeight ), Colors::LightGray, UNDESTROYABLE );
                }
                else
                {
                    if( ( 0 == y || 2 == y ) && x % 2 )
                    {
                        bricks[ y * nBricksAcross + x ] = Brick( RectF( topLeft + Vec2( x * brickWidth, y * brickHeight ),
                                                                        brickWidth, brickHeight ), c, SOLID, 2 );
                        nBricksLeft++;
                    }
                    else
                    {
                        bricks[ y * nBricksAcross + x ] = Brick( RectF( topLeft + Vec2( x * brickWidth, y * brickHeight ), brickWidth, brickHeight ), c );
                        nBricksLeft++;
                    }
                }
            }
        }
    }
    else if( 2 == level )
    {
        nBricksLeft = 0;
        for( int y = 0; y < nBricksDown; ++y )
        {
            const Color c = brickColors[ 3 - y ];
            for( int x = 0; x < nBricksAcross; ++x )
            {
                if( y % 2 - 1 && x % 2 - 1 )
                {
                  // empty space
                }
                else
                {
                    if( ( 0 == y || 2 == y ) && x % 2 )
                    {
                        bricks[ y * nBricksAcross + x ] = Brick( RectF( topLeft + Vec2( x * brickWidth, y * brickHeight ),
                                                                        brickWidth, brickHeight ), c, SOLID, 2 );
                        nBricksLeft++;
                    }
                    else
                    {
                        bricks[ y * nBricksAcross + x ] = Brick( RectF( topLeft + Vec2( x * brickWidth, y * brickHeight ), brickWidth, brickHeight ), c );
                        nBricksLeft++;
                    }
                }
            }
        }
        bricks[ 0 ] = Brick( RectF( topLeft + Vec2( -brickWidth, ( nBricksDown - 1 ) * brickHeight ),
                                    brickWidth, brickHeight ), Colors::LightGray, UNDESTROYABLE );
        bricks[ 2 ] = Brick( RectF( topLeft + Vec2( nBricksAcross * brickWidth, ( nBricksDown - 1 ) * brickHeight ),
                                    brickWidth, brickHeight ), Colors::LightGray, UNDESTROYABLE );
    }
}

void Game::Go()
{
    gfx.BeginFrame();
    float elapsedTime = ft.Mark();
    while( elapsedTime > 0.0f )
    {
        const float dt = std::min( 0.0025f, elapsedTime );
        UpdateModel( dt );
        elapsedTime -= dt;
    }
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel( float dt )
{
    if( wnd.kbd.KeyIsPressed( VK_SPACE ) )
    {
        ball.Start();
    }

    // Restart game
    if( wnd.kbd.KeyIsPressed( VK_RETURN ) )
    {
        level = 0;
        ResetGame();
    }

    if( lifes > 0 && nBricksLeft > 0 )
    {
        /////////////////
        //// PADDLE /////
        /////////////////
        if( ball.GetState() != WAITING )
        {
            pad.Update( wnd.kbd, dt );
        }
        pad.DoWallCollision( walls.GetInnerBounds() );
        if( pad.DoBallCollision( ball ) )
        {
            soundPad.Play();
        }

        //////////////////////
        //// LASER SHOTS /////
        //////////////////////
        if( pad.HasLaserGun() )
        {
            Shoot();
        }
        else
        {
            startedShooting = false;
        }
        for( int i = 0; i < nMaxLaserShots; ++i )
        {
            laserShots[ i ].Update( dt );
        }
        
        ////////////////////
        //// POWER UPS /////
        ////////////////////
        for( int i = 0; i < nPowerUps; ++i )
        {
            if( true == powerUps[ i ].Update( pad.GetRect(), dt ) )
            {
                powerUpSounds[ i ].Play( 1, 1.5F );
                ApplyPowerUp( powerUps[ i ] );
            }
        }

        /////////////////
        //// BRICKS /////
        /////////////////
        bool collisionHappened = false;
        float curColDistSq;
        int curColIdx;
        for( int i = 0; i < nBricks; ++i )
        {
            // ball collision
            if( bricks[ i ].CheckBallCollision( ball ) )
            {
                const float newColDistSq = ( ball.GetPosition() - bricks[ i ].GetCenter() ).GetLengthSq();
                if( collisionHappened )
                {
                    if( newColDistSq < curColDistSq )
                    {
                        curColDistSq = newColDistSq;
                        curColIdx = i;
                    }
                }
                else
                {
                    curColDistSq = newColDistSq;
                    curColIdx = i;
                    collisionHappened = true;
                }
            }

            // laser collision
            for( int ls = 0; ls < nMaxLaserShots; ++ls )
            {
                if( laserShots[ ls ].IsActivated() && bricks[ i ].CheckLaserCollision( laserShots[ ls ] ) )
                {
                    nBricksLeft--;
                    CreatePowerUp( i );
                    //TODO add sound for collision
                }
            }
        }
        if( collisionHappened )
        {
            pad.ResetCooldown();
            if( bricks[ curColIdx ].ExecuteBallCollision( ball ) )
            {
                nBricksLeft--;
                CreatePowerUp( curColIdx );
            }
            
            soundBrick.Play();
        }
        if( nBricksLeft <= 0 )
        {
            soundVictory.Play();
            startTime_levelFinished = std::chrono::steady_clock::now();
        }

        //////////////////////
        //// BALL & LIFE /////
        //////////////////////
        ball.Update( dt, pad.GetRect().GetCenter().x, wnd.kbd );
        const int collResult = ball.DoWallCollision( walls.GetInnerBounds() );
        if( 1 == collResult )
        {
            // only reset cooldown if not still coliding with ball
            // (helps prevent weird shit when ball is trapped against wall)
            if( !pad.GetRect().IsOverlappingWith( ball.GetRect() ) )
            {
                pad.ResetCooldown();
            }
        }
        else if( 2 == collResult )
        {
            lifes--;
            if( 0 == lifes )
            {
                soundGameOver.Play();
            }
            else
            {
                ResetPaddle();
                ResetBall();
                ResetPowerUps();
                ResetShots();
                soundLifeLoss.Play();
            }
        }
    }
    else if( nBricksLeft <= 0 )
    {
        // next level
        const std::chrono::duration<float> timeElapsed = std::chrono::steady_clock::now() - startTime_levelFinished;
        if( timeElapsed.count() > timeBetweenLevels )
        {
            level++;
            ResetPaddle();
            ResetBall();
            ResetPowerUps();
            ResetShots();
            CreateNextLevel();
        }
    }
}

void Game::DrawGameOver()
{
    Vec2 topLeft( 80, 80 );
    float rectWidth = gfx.ScreenWidth - 2 * topLeft.x;

    while( rectWidth > 250 )
    {
        Vec2 bottomRight( gfx.ScreenWidth - topLeft.x, gfx.ScreenHeight - topLeft.y );
        gfx.DrawRectBorder( RectF( topLeft, bottomRight ), 4, Colors::Cyan );
        topLeft += Vec2( 60, 60 );
        rectWidth = bottomRight.x - topLeft.x;
    }
}

void Game::DrawVictory()
{
    Vec2 topLeft( 70, 70 );
    float rectWidth = gfx.ScreenWidth - 2 * topLeft.x;

    while( rectWidth > 250 )
    {
        Vec2 bottomRight( gfx.ScreenWidth - topLeft.x, gfx.ScreenHeight - topLeft.y );
        gfx.DrawRectBorder( RectF( topLeft, bottomRight ), 4, Colors::MakeRGB( rand() % 255, rand() % 255, rand() % 255 ) );
        topLeft += Vec2( 30, 30 );
        rectWidth = bottomRight.x - topLeft.x;
    }
}

void Game::ComposeFrame()
{
    for( const Brick& b : bricks )
    {
        b.Draw( gfx );
    }

    pad.Draw( gfx );
    walls.Draw( gfx );
    pad.DrawAsLifesRemaining( gfx, lifes, Vec2( walls.GetInnerBounds().left, 15 ), 0.3f );

    if( 0 == lifes )
    {
        DrawGameOver();
    }
    else
    {
        ball.Draw( gfx );

        for( int i = 0; i < nPowerUps; ++i )
        {
            powerUps[ i ].Draw( gfx );
        }

        for( int i = 0; i < nMaxLaserShots; ++i )
        {
            laserShots[ i ].Draw( gfx );
        }
    }
    if( 0 == nBricksLeft )
    {
        DrawVictory();
    }
}
