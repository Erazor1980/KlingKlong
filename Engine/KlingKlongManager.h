#pragma once
#include <chrono>
#include "Graphics.h"
#include "Sound.h"
#include "Keyboard.h"

enum class GameState
{
    START_SCREEN,
    /*MAIN_MENU,*/
    PLAYING,
    GAME_OVER,
    EXIT_GAME
};


class KlingKlongManager
{
public:
    KlingKlongManager( Graphics& gfx_in, GameState& gameState );

    void Update( Keyboard& kbd );
    void DrawScene() const;
private:
    enum Selection
    {
        START_GAME = 0,
        /*DIFFICULTY,*/
        EXIT,
        NUM_SELECTIONS
    };
    void UpdateStartScreen( Keyboard& kbd );

    /* START_SCREEN */
    std::chrono::steady_clock::time_point startTime_startScreen;
    int startScreenCnt = -1;    /* for drawing/playing title screen: 0 -> "welcome to", 1 -> "kling", 2 -> "klong", 3 -> "press key" */
    Selection optionSelected = Selection::START_GAME;

    /* ALL SOUNDS */
    Sound soundWelcomeTo    = Sound( L"Sounds\\welcomeTo.wav" );
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
    Surface sur_Background  = Surface::FromFile( L"Images\\background.png" );
    Surface sur_KlingKlong  = Surface::FromFile( L"Images\\Text\\KlingKlong.png" );
    Surface sur_WelcomeTo   = Surface::FromFile( L"Images\\Text\\welcome.png" );
    Surface sur_StartGame   = Surface::FromFile( L"Images\\Text\\startGame.png" );
    Surface sur_Exit        = Surface::FromFile( L"Images\\Text\\exit.png" );
    Surface sur_Difficulty  = Surface::FromFile( L"Images\\Text\\difficulty.png" );
    Surface sur_Easy        = Surface::FromFile( L"Images\\Text\\easy.png" );
    Surface sur_Medium      = Surface::FromFile( L"Images\\Text\\medium.png" );
    Surface sur_Hard        = Surface::FromFile( L"Images\\Text\\hard.png" );

    Font font;
    GameState& gameState;
    Graphics& gfx;
};