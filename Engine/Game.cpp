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
    walls( RectF::FromCenter( Graphics::GetScreenRect().GetCenter(), fieldWidth / 2.0f, fieldHeight / 2.0f ), wallThickness, wallColor )
{
    powerUps[ 0 ] = PowerUp( brickWidth, brickHeight, INCR_PADDLE_SIZE, 10, walls.GetInnerBounds().bottom );
    powerUps[ 1 ] = PowerUp( brickWidth, brickHeight, EXTRA_LIFE, 0, walls.GetInnerBounds().bottom );
    // not implemented yet!
    powerUps[ 2 ] = PowerUp( brickWidth, brickHeight, CANNON, 10, walls.GetInnerBounds().bottom );

    powerUpSounds[ 0 ] = Sound( L"Sounds\\grow.wav" );
    powerUpSounds[ 1 ] = Sound( L"Sounds\\extraLife.wav" );

    ResetGame();
}

void Game::ResetGame()
{
    // reset paddle
    ResetPaddle();

    // reset ball
    ResetBall();
    
    // reset bricks
    const Vec2 topLeft( walls.GetInnerBounds().left + distWallBricks, walls.GetInnerBounds().top + 50 );
    nBricksLeft = nBricks;
    for( int y = 0; y < nBricksDown; ++y )
    {
        const Color c = brickColors[ y ];
        for( int x = 0; x < nBricksAcross; ++x )
        {
            if( 0 == x || nBricksAcross - 1 == x )
            {
                bricks[ y * nBricksAcross + x ] = Brick( RectF( topLeft + Vec2( x * brickWidth, y * brickHeight ),
                                                                brickWidth, brickHeight ), Colors::LightGray, UNDESTROYABLE );
                nBricksLeft--;
            }
            else
            {
                if( 3 == y && x % 2 )
                {
                    bricks[ y * nBricksAcross + x ] = Brick( RectF( topLeft + Vec2( x * brickWidth, y * brickHeight ),
                                                                    brickWidth, brickHeight ), Colors::MakeRGB( 205, 102, 29 ), SOLID, 2 );
                }
                else
                {
                    bricks[ y * nBricksAcross + x ] = Brick( RectF( topLeft + Vec2( x * brickWidth, y * brickHeight ), brickWidth, brickHeight ), c );
                }                
            }
        }
    }

    // reset life
    lifes = MAX_LIFES;

    // reset power ups
    ResetPowerUps();

    std::srand( ( unsigned int )std::time( 0 ) );
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
    case CANNON:
        break;
    default:
        break;
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
        }
        if( collisionHappened )
        {
            pad.ResetCooldown();
            if( bricks[ curColIdx ].ExecuteBallCollision( ball ) )
            {
                nBricksLeft--;

                ////////////////////
                //// POWER UPS /////
                ////////////////////
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
                if( rand() % 10 == 1 )
                {
                    powerUps[ 0 ].Activate( bricks[ curColIdx ].GetCenter() - Vec2( brickWidth / 2, 0 ) );
                }
                else if( lifes < MAX_LIFES && rand() % 10 == 1 )
                {
                    powerUps[ 1 ].Activate( bricks[ curColIdx ].GetCenter() - Vec2( brickWidth / 2, 0 ) );
                }
#endif
            }
            
            soundBrick.Play();
            if( 0 == nBricksLeft )
            {
                soundVictory.Play();
            }
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
                soundLifeLoss.Play();
            }
        }
    }
    else
    {
        if( wnd.kbd.KeyIsPressed( VK_RETURN ) )
        {
            ResetGame();
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
    }
    if( 0 == nBricksLeft )
    {
        DrawVictory();
    }
}
