#pragma once
#include <chrono>
#include "Graphics.h"
#include "Sound.h"
#include "Keyboard.h"
#include "Brick.h"
#include "Ball.h"
#include "Walls.h"
#include "PowerUp.h"
#include "Paddle.h"
#include "Enemy.h"
#include "Laser.h"

enum class GameState
{
    START_SCREEN,
    /*MAIN_MENU,*/
    PLAYING,
    GAME_OVER,
    VICTORY_SCREEN,
    EXIT_GAME
};


class KlingKlongManager
{
public:
    KlingKlongManager( Graphics& gfx_in, GameState& gameState_in, const Walls& walls_in );

    void Update( const float dt, Keyboard& kbd );
    void DrawScene();
private:
    void UpdateStartScreen( Keyboard& kbd );
    void KeyHandling( Keyboard& kbd );
    void ApplyPowerUp( const PowerUp& pu );
    void CreateMultiBalls();
    void CreatePowerUp( const Vec2& pos, const bool enemyKilled );
    void AddPowerUp( const ePowerUpType& type, const Vec2& posToSpawn );
    void ShootLaser();
    void SpawnEnemy( const Vec2& pos = Vec2( Graphics::ScreenWidth / 2, Graphics::ScreenHeight / 2 ) );
    void UpdateBalls( const float dt, Keyboard& kbd );
    void UpdatePaddle( const float dt, Keyboard& kbd );
    void UpdateBricks( const float dt );
    void UpdatePowerUps( const float dt );
    void UpdateLaserShots( const float dt );
    void UpdateEnemies( const float dt );

    void ResetGame();
    void ResetBall();
    void ResetPaddle();

    void CreateNextLevel();
    void DrawLightning();
private:
    enum Selection
    {
        START_GAME = 0,
        /*DIFFICULTY,*/
        //CONTINUE_GAME,
        //RESTART_GAME,
        EXIT,
        NUM_SELECTIONS
    };

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
    Sound soundPU_incrSize  = Sound( L"Sounds\\grow.wav" );
    Sound soundPU_extraLife = Sound( L"Sounds\\extraLife.wav" );
    Sound soundPU_multiBall = Sound( L"Sounds\\multiBall.wav" );
    Sound soundPU_superBall = Sound( L"Sounds\\superBall.wav" );

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
    Surface sur_GameOver    = Surface::FromFile( L"Images\\Text\\gameover.png" );
    Surface sur_Victory     = Surface::FromFile( L"Images\\Text\\victory.png" );
    static constexpr int nSubImagesInSequence = 8;  /* first 4 sequences consist of 8 images */
    static constexpr int nPowerUps = 5;
    Surface PowerUpSequences[ nPowerUps ] = { Surface::FromFile( L"Images\\Sequences\\incrSize.png" ),
                                              Surface::FromFile( L"Images\\Sequences\\extraLife.png" ),
                                              Surface::FromFile( L"Images\\Sequences\\laserGun.png" ),
                                              Surface::FromFile( L"Images\\Sequences\\multiBall.png" ),
                                              Surface::FromFile( L"Images\\Sequences\\superBall.png" ) };
    Surface PadExplosion[ 4 ] = { Surface::FromFile( L"Images\\padExpl1.png" ),
                                  Surface::FromFile( L"Images\\padExpl2.png" ),
                                  Surface::FromFile( L"Images\\padExpl3.png" ),
                                  Surface::FromFile( L"Images\\padExpl4.png" ) };
    int explSeqIdx = 0;
    Surface seqEnemy    = Surface::FromFile( L"Images\\Sequences\\funnyEnemy.png" );
    Surface seqLightning   = Surface::FromFile( L"Images\\Sequences\\lightning.png" );
    int lightningSeqIdx = 0;
#if EASY_MODE
    float timeBetweenEnemies = 15;  /* in seconds */
#else
    float timeBetweenEnemies = 10;  /* in seconds */
#endif

    float timeBetweenLevels = 3;    /* in seconds */
    /* TIMER */
    std::chrono::steady_clock::time_point startTime_enemySpawn;
    std::chrono::steady_clock::time_point startTime_shot;
    std::chrono::steady_clock::time_point startTime_explosion;
    std::chrono::steady_clock::time_point startTime_victory;
    std::chrono::steady_clock::time_point startTime_lightning;

    bool startedShooting = false;
    static constexpr float timeBetweenShots = 0.35f;     /* in seconds */

    /* BALLS, BRICKS, ENEMIES, LASERSHOTS */
    std::vector< Ball > vBalls;
    std::vector< Brick > vBricks;
    std::vector< PowerUp > vPowerUps;
    std::vector< Enemy > vEnemies;
    std::vector< LaserShot > vLaserShots;

    int lifes = MAX_LIFES;
    int level = 0;
    std::vector< std::string > allLevels;
    int nBricksLeft = 0;

    //Font font;
    Paddle pad;
    GameState& gameState;
    Graphics& gfx;
    const Walls& walls;

    Font font;
    Surface fontSurface = Surface::FromFile( L"Images\\Text\\Fixedsys16x28.bmp" );
};