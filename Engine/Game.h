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

#define MAX_LIFES 5
#define BOARD_PADDING 40 /* in pixel from screen border to game border */

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
    void DrawLifesLeft();
    void ResetGame();
    void ResetBall();
    void ResetPaddle();
	/********************************/
private:
	MainWindow& wnd;
	Graphics gfx;
	/********************************/
	/*  User Variables              */
    static constexpr float brickWidth = 40.0f;
    static constexpr float brickHeight = 24.0f;
    static constexpr int nBricksAcross = 18;
    static constexpr int nBricksDown = 4;
    static constexpr int nBricks = nBricksDown * nBricksAcross;
    int nBricksLeft = nBricks;  /* only destructible bricks! */
    FrameTimer ft;
    Ball ball;
    Paddle pad;
    RectF walls;
    Brick bricks[ nBricks ];
    Sound soundPad;
    Sound soundBrick;
    Sound soundGameOver;
    Sound soundLifeLoss;
    Sound soundVictory;
    int lifes = MAX_LIFES;
	/********************************/
};