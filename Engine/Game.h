/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.h																				  *
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
#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include "FrameTimer.h"
#include "Ball.h"
#include "Sound.h"
#include "Brick.h"
#include "Paddle.h"
#include "Walls.h"
#include "PowerUp.h"

#define MAX_LIFES 1
//#define BOARD_PADDING 40 /* in pixel from screen border to game border */

class Game
{
public:
	Game( class MainWindow& wnd );
	Game( const Game& ) = delete;
	Game& operator=( const Game& ) = delete;
	void Go();
private:
	void ComposeFrame();
	void UpdateModel( float dt );
	/********************************/
	/*  User Functions              */
    void DrawGameOver();
    void DrawVictory();
    void ResetGame();
    void ResetBall();
    void ResetPaddle();
    void ApplyPowerUp( const PowerUp& pu );
	/********************************/
private:
	MainWindow& wnd;
	Graphics gfx;
	/********************************/
	/*  User Variables              */
    static constexpr float brickWidth = 56.0f;
    static constexpr float brickHeight = 22.0f;
    static constexpr int nBricksAcross = 12;
    static constexpr int nBricksDown = 4;
    static constexpr int nBricks = nBricksDown * nBricksAcross;
    int nBricksLeft = nBricks;  /* only destructible bricks! is updated while creating bricks in ResetGame() */

    static constexpr float wallThickness = 12.0f;
    static constexpr float fieldWidth = float( nBricksAcross ) * brickWidth + 100;
    static constexpr float fieldHeight = float( Graphics::ScreenHeight ) - 100;
    static constexpr float distWallBricks = ( fieldWidth - brickWidth * nBricksAcross ) / 2.0f;
    static constexpr Color wallColor = { 20, 60, 200 };
    static constexpr Color brickColors[ 4 ] = { { 230, 0, 0 }, { 0, 230, 0 }, { 0, 0, 230 }, { 0, 230 ,230 } };

    FrameTimer ft;
    Ball ball;
    Paddle pad;
    Walls walls;
    Brick bricks[ nBricks ];
    Sound soundPad;
    Sound soundBrick;
    Sound soundGameOver;
    Sound soundLifeLoss;
    Sound soundVictory;

    static constexpr int nPowerUps = 3;     /* maximal number of powerups */
    PowerUp powerUps[ nPowerUps ];
    Sound powerUpSounds[ nPowerUps ];
    int lifes = MAX_LIFES;
	/********************************/
};