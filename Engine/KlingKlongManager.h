#pragma once
#include <chrono>
#include "Graphics.h"
#include "Sound.h"

enum class GameState
{
    START_SCREEN,
    MAIN_MENU,
    PLAYING,
    GAME_OVER
};

class KlingKlongManager
{
public:
    KlingKlongManager( Graphics& gfx_in, GameState& gameState );

    void Update();
    void DrawScene() const;
private:
    /* START_SCREEN */
    std::chrono::steady_clock::time_point startTime_startScreen;
    int cnt = 0;    /* 0 -> "welcome to", 1 -> "kling", 2 -> "klong" */

    /* ALL SOUNDS */
    Sound soundPad          = Sound( L"Sounds\\arkpad.wav" );
    Sound soundBrick        = Sound( L"Sounds\\arkbrick.wav" );
    Sound soundBrick2       = Sound( L"Sounds\\arkbrick2.wav" );
    Sound soundLifeLoss     = Sound( L"Sounds\\fart1.wav" );
    Sound soundGameOver     = Sound( L"Sounds\\explosion.wav" );
    Sound soundVictory      = Sound( L"Sounds\\victory.wav" );
    Sound soundLaserShot    = Sound( L"Sounds\\laserShot.wav" );
    Sound soundEnemyHit     = Sound( L"Sounds\\hitEnemy.wav" );
    Sound soundKillEnemy    = Sound( L"Sounds\\killEnemy.wav" );

    /* ALL IMAGES */
    Surface Background      = Surface::FromFile( L"Images\\background.png" );
    Surface KlingKlong      = Surface::FromFile( L"Images\\KlingKlong.png" );
    Surface WelcomeTo       = Surface::FromFile( L"Images\\welcome.png" );

    Font font;
    GameState& gameState;
    Graphics& gfx;
};