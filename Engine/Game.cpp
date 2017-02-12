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
    pad( Vec2( 400, 500 ), 70, 15 )
{
    ResetGame();
}

void Game::ResetGame()
{
    ball = Ball( Vec2( 300 + 24, 300 ), Vec2( -3, -3 ) );

    ;
    walls = RectF( Vec2( 40, 40 ), Vec2( gfx.ScreenWidth - 40, gfx.ScreenHeight - 40 ) );

    const Color colors[ 4 ] ={ Colors::Red, Colors::Green, Colors::Blue, Colors::Cyan };
    const Vec2 topLeft( 40, 40 );

    for( int y = 0; y < nBricksDown; ++y )
    {
        const Color c = colors[ y ];
        for( int x = 0; x < nBricksAcross; ++x )
        {
            bricks[ y * nBricksAcross + x ] = Brick( RectF( topLeft + Vec2( x * brickWidth, y * brickHeight ), brickWidth, brickHeight ), c );
        }
    }

    lifes = MAX_LIFES;
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
    if( wnd.kbd.KeyIsPressed( VK_RETURN ) )
    {
        ResetGame();
    }

    if( lifes > 0 )
    {
        pad.Update( wnd.kbd, dt );
        pad.DoWallCollision( walls );
        ball.Update( dt );

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
            soundBrick.Play();
        }

        if( pad.DoBallCollision( ball ) )
        {
            soundPad.Play();
        }

        if( ball.DoWallCollision( walls ) )
        {
            pad.ResetCooldown();
        }
    }
}

void Game::DrawGameOver()
{
    Vec2 topLeft( 20, 20 );
    float rectWidth = gfx.ScreenWidth - 2 * topLeft.x;

    while( rectWidth > 250 )
    {
        Vec2 bottomRight( gfx.ScreenWidth - topLeft.x, gfx.ScreenHeight - topLeft.y );
        gfx.DrawRectBorder( RectF( topLeft, bottomRight ), 4, Colors::Cyan ); //Colors::MakeRGB( rand() % 255, rand() % 255, rand() % 255 ) );
        topLeft += Vec2( 30, 30 );
        rectWidth = bottomRight.x - topLeft.x;
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

    gfx.DrawRectBorder( RectF( Vec2( 40, 40 ), Vec2( gfx.ScreenWidth - 40, gfx.ScreenHeight - 40 ) ), 4, Colors::Red );

    if( lifes == 0 )
    {
        DrawGameOver();
    }
}
