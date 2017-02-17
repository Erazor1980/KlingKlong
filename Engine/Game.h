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
#include "Laser.h"

#define MAX_LIFES 3

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
    void ResetPowerUps();
    void ResetShots();
    void ApplyPowerUp( const PowerUp& pu );
    void Shoot();
    void CreatePowerUp( int curColIdx );
    void CreateNextLevel();
    void CreateMultiBalls();
    void UpdateMultiBalls( int idxBallToDeactivate );
	/********************************/
private:
	MainWindow& wnd;
	Graphics gfx;
	/********************************/
	/*  User Variables              */
    static constexpr float brickWidth = 56.0f;
    static constexpr float brickHeight = 22.0f;
    static constexpr int nBricksAcross = 13;
    static constexpr int nBricksDown = 4;
    static constexpr int nBricks = nBricksDown * nBricksAcross;
    int nBricksLeft = nBricks;  /* only destructible bricks! is updated while creating bricks in ResetGame() */

    static constexpr float wallThickness = 12.0f;
    static constexpr float fieldWidth = float( Graphics::ScreenWidth ) - 50;
    static constexpr float fieldHeight = float( Graphics::ScreenHeight ) - 100;
    static constexpr float distWallBricks = ( fieldWidth - brickWidth * nBricksAcross ) / 2.0f;
    static constexpr Color wallColor = { 20, 60, 200 };
    static constexpr Color brickColors[ 4 ] = { { 230, 0, 0 }, { 0, 230, 0 }, { 0, 0, 230 }, { 0, 230 ,230 } };

    FrameTimer ft;
    Paddle pad;
    Walls walls;
    Brick bricks[ nBricks ];
    Sound soundPad;
    Sound soundBrick;
    Sound soundBrick2;
    Sound soundGameOver;
    Sound soundLifeLoss;
    Sound soundVictory;
    Sound soundLaserShot;

    ////////////////////
    //// POWER UPS /////
    ////////////////////
    static constexpr int nPowerUps = 4;             /* maximal number of powerups */
    PowerUp powerUps[ nPowerUps ];
    Sound powerUpSounds[ nPowerUps ];
    /* laser */
    static constexpr int nMaxLaserShots = 20;           /* maximal number of laser shots */
    LaserShot laserShots[ nMaxLaserShots ];
    std::chrono::steady_clock::time_point startTime_shot;
    bool startedShooting = false;
    static constexpr float timeBetweenShots = 0.35f;     /* in seconds */
    int shotIdx = 0;                                    /* idx to add next shot */
    /* multi ball */
    static constexpr int nMaxBalls = 3;           /* maximal number of balls */
    Ball balls[ nMaxBalls ];
    bool multiBalls = false;
    int lastBallIdx = 0;

    //TODO add other seq images!
    static constexpr int nSubImagesInSequence = 8;  /* all sequences will consist of 8 images */
    Surface PowerUpSequences[ nPowerUps ] = { Surface::FromFile( L"Images\\Sequences\\extraLife.png" ),
                                              Surface::FromFile( L"Images\\Sequences\\extraLife.png" ),
                                              Surface::FromFile( L"Images\\Sequences\\extraLife.png" ),
                                              Surface::FromFile( L"Images\\Sequences\\extraLife.png" ) };

    Surface GameOverSurf = Surface::FromFile( L"Images\\gameover.png" );
    Surface PadExplosion[ 4 ] = { Surface::FromFile( L"Images\\padExpl1.png" ),
                                  Surface::FromFile( L"Images\\padExpl2.png" ),
                                  Surface::FromFile( L"Images\\padExpl3.png" ),
                                  Surface::FromFile( L"Images\\padExpl4.png" ) };
    std::chrono::steady_clock::time_point startTime_explosion;
    int explSeqIdx = 0;

    int lifes = MAX_LIFES;
    int level = 0;
    float timeBetweenLevels = 3;    /* in seconds */
    std::chrono::steady_clock::time_point startTime_levelFinished;
	/********************************/
};