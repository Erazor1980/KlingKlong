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

Game::Game( MainWindow& wnd )
    :
    wnd( wnd ),
    gfx( wnd ),
    soundPad( L"Sounds\\arkpad.wav" ),
    soundBrick( L"Sounds\\arkbrick.wav" ),
    soundLifeLoss( L"Sounds\\fart1.wav" ),
    soundGameOver( L"Sounds\\gameover.wav" ),
    soundVictory( L"Sounds\\victory.wav" )
{
    walls = RectF( Vec2( BOARD_PADDING, BOARD_PADDING ), Vec2( gfx.ScreenWidth - BOARD_PADDING, gfx.ScreenHeight - BOARD_PADDING ) );
    ResetGame();
}

void Game::ResetGame()
{
    // reset paddle
    ResetPaddle();

    // reset ball
    ResetBall();
    
    // reset bricks
    const Color colors[ 4 ] ={ Colors::Red, Colors::Green, Colors::Blue, Colors::Cyan };
    const Vec2 topLeft( 90, 90 );

    for( int y = 0; y < nBricksDown; ++y )
    {
        const Color c = colors[ y ];
        for( int x = 0; x < nBricksAcross; ++x )
        {
            bricks[ y * nBricksAcross + x ] = Brick( RectF( topLeft + Vec2( x * brickWidth, y * brickHeight ), brickWidth, brickHeight ), c );
        }
    }
    nBricksLeft = nBricks;

    // reset life
    lifes = MAX_LIFES;
}

void Game::ResetBall()
{
    ball = Ball( Vec2( pad.GetRect().GetCenter().x, pad.GetRect().top - 7 ) , Vec2( 0.001, -3 ) );
    ball.Stop();
}

void Game::ResetPaddle()
{
    pad = Paddle( Vec2( gfx.ScreenWidth / 2, gfx.ScreenHeight - BOARD_PADDING - 50 ), 70, 15 );
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

    //TODO remove later! for testing
    if( wnd.kbd.KeyIsPressed( VK_RETURN ) )
    {
        ResetGame();
    }
    
    if( lifes > 0 && nBricksLeft > 0 )
    {
        pad.Update( wnd.kbd, dt );
        pad.DoWallCollision( walls );
        ball.Update( dt, pad.GetRect().GetCenter() );

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
            bricks[ curColIdx ].ExecuteBallCollision( ball );
            nBricksLeft--;
            if( 0 == nBricksLeft )
            {
                soundVictory.Play();
            }
            else
            {
                soundBrick.Play();
            }
        }

        if( pad.DoBallCollision( ball ) )
        {
            soundPad.Play();
        }

        if( ball.DoWallCollision( walls ) )
        {
            pad.ResetCooldown();
        }

        if( ball.GetPosition().y >= walls.bottom - 10 )
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
        gfx.DrawRectBorder( RectF( topLeft, bottomRight ), 4, Colors::Cyan ); //Colors::MakeRGB( rand() % 255, rand() % 255, rand() % 255 ) );
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

void Game::DrawLifesLeft()
{
    const int radius = BOARD_PADDING / 4;
    for( int i = 1; i <= lifes; ++i )
    {
        gfx.DrawCircle( i * radius * 3 + 40, BOARD_PADDING / 2, radius, Colors::Red );
    }
}

void Game::ComposeFrame()
{
    ball.Draw( gfx );
    for( const Brick& b : bricks )
    {
        b.Draw( gfx );
    }
    pad.Draw( gfx );

    gfx.DrawRectBorder( walls, 3, Colors::LightGray );

    DrawLifesLeft();

    if( 0 == lifes )
    {
        DrawGameOver();
    }
    if( 0 == nBricksLeft )
    {
        DrawVictory();
    }
}
