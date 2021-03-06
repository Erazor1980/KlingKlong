#include "KlingKlongManager.h"
#include <ctime>

#include <fstream>
// laoding level design from file
std::vector< std::string > LoadTextFile( std::string filename )
{
    std::fstream txtfile( filename.c_str() );
    assert( txtfile.good() );
    std::vector<std::string> text;
    if( txtfile.good() )
    {
        std::string str;
        while( std::getline( txtfile, str ) )
        {
            text.push_back( str );
        }
        txtfile.close();
    }
    return std::move( text );
}

KlingKlongManager::KlingKlongManager( Graphics& gfx_in, GameState& gameState_in, const Walls& walls_in )
    :
    gfx( gfx_in ),
    gameState( gameState_in ),
    walls( walls_in )
{
#if _DEBUG
    startScreenCnt = 3;
#endif

    allLevels = LoadTextFile( "level.txt" );

    ResetGame();
}

void KlingKlongManager::ResetGame()
{
    // seed rand()
    std::srand( ( unsigned int )std::time( 0 ) );

    // level
    level = 0;

    // reset paddle
    ResetPaddle();

    // reset ball
    ResetBall();

    // reset bricks
    CreateNextLevel();

    // reset life, max enemies & time between enemies
    switch( difficulty )
    {
    case KlingKlongManager::EASY:
        maxLifes = 5;
        lifes = 3;
        maxEnemies = 2;
        timeBetweenEnemies = 15;
        break;
    case KlingKlongManager::MEDIUM:
        maxLifes = 4;
        lifes = 3;
        maxEnemies = 3;
        timeBetweenEnemies = 10;
        break;
    case KlingKlongManager::HARD:
        maxLifes = 2;
        lifes = 2;
        maxEnemies = 5;
        timeBetweenEnemies = 6;
        break;
    case KlingKlongManager::INSANE:
        maxLifes = 1;
        lifes = 1;
        maxEnemies = 10;
        timeBetweenEnemies = 2;
        break;
    default:
        break;
    }

    // reset power ups
    vPowerUps.clear();

    // reset shots
    vLaserShots.clear();

    // reset enemies
    vEnemies.clear();

    // explosion sequence
    explSeqIdx = 0;

    // reset statistics
    stats.reset();

    // reset timer
    startTime_shot          = std::chrono::steady_clock::now();
    startTime_enemySpawn    = std::chrono::steady_clock::now();
    
    /////////////////
    //// TESTING ////
    /////////////////
    //AddPowerUp( INCR_PADDLE_SIZE, Vec2( 450, 100 ) );
    //AddPowerUp( EXTRA_LIFE, Vec2( 450, 100 ) );
    //AddPowerUp( LASER_GUN, Vec2( 450, 500 ) );
    //AddPowerUp( MULTI_BALL, Vec2( 450, 100 ) );
    //AddPowerUp( SUPER_BALL, Vec2( 450, 100 ) );
    //vBalls[ 0 ].StickToPaddle( pad.GetRect().GetCenter().x );
    //nBricksLeft = 1;
    //vBalls[ 0 ].ActivateSuperBall( 5, ( PowerUpSequences[ SUPER_BALL ].GetWidth() / 5 ) / 2.0f ) ;
}

void KlingKlongManager::Update( const float dt, Keyboard& kbd )
{
    switch( gameState )
    {
    case GameState::START_SCREEN:
        UpdateStartScreen( kbd );
        break;
    case GameState::PLAYING:
    {
        KeyHandling( kbd );

        UpdateBalls( dt, kbd );

        UpdatePaddle( dt, kbd );
        UpdatePowerUps( dt );
        UpdateBricks( dt );
        UpdateLaserShots( dt );
        UpdateEnemies( dt );

        if( lifes <= 0 )
        {
            soundGameOver.Play( 1, 0.5f );
            startTime_explosion = std::chrono::steady_clock::now();
            gameState = GameState::GAME_OVER;
        }
        else if( 0 == nBricksLeft )
        {
            soundVictory.Play();
            gameState = GameState::VICTORY_SCREEN;
            startTime_victory = std::chrono::steady_clock::now();
        }
    }
    break;
    case GameState::GAME_OVER:
    {
        gameStarted = false;
        const std::chrono::duration<float> timeElapsed = std::chrono::steady_clock::now() - startTime_explosion;
        if( explSeqIdx < 4 && timeElapsed.count() > 0.15f )
        {
            explSeqIdx++;
            startTime_explosion = std::chrono::steady_clock::now();
        }
        if( explSeqIdx >= 4 )
        {
            KeyHandling( kbd );
        }
        if( timeElapsed.count() > 2.5f )
        {
            gameState = GameState::STATISTICS_SCREEN;
        }
    }
    break;
    case GameState::VICTORY_SCREEN:
    {
        // next level
        const std::chrono::duration<float> timeElapsed = std::chrono::steady_clock::now() - startTime_victory;
        if( timeElapsed.count() > timeBetweenLevels )
        {
            level++;
            ResetPaddle();
            ResetBall();
            vPowerUps.clear();
            vLaserShots.clear();
            vEnemies.clear();
            CreateNextLevel();
            startTime_enemySpawn = std::chrono::steady_clock::now();
            gameState = GameState::PLAYING;
        }
    }
    break;
    case GameState::STATISTICS_SCREEN:
    {
        KeyHandling( kbd );
    }
    break;
    default:
        break;
    }
}

void KlingKlongManager::UpdateStartScreen( Keyboard& kbd )
{
    /* only for the very beginning of the game */
    if( startScreenCnt < 3 )
    {
        if( -1 == startScreenCnt )
        {
            startTime_startScreen = std::chrono::steady_clock::now();
            soundWelcomeTo.Play();
            startScreenCnt = 0;
        }
        const std::chrono::duration<float> timeElapsed = std::chrono::steady_clock::now() - startTime_startScreen;
        if( 0 == startScreenCnt && timeElapsed.count() >= 2 )
        {
            startScreenCnt = 1;
            startTime_startScreen = std::chrono::steady_clock::now();
            soundBrick.Play();
        }
        else if( 1 == startScreenCnt && timeElapsed.count() >= 1 )
        {
            startScreenCnt = 2;
            startTime_startScreen = std::chrono::steady_clock::now();
            soundPad.Play();
        }
        else if( 2 == startScreenCnt && timeElapsed.count() >= 0.5f )
        {
            startScreenCnt = 3;
        }
    }

    /* every time we are in the start menu */
    while( !kbd.KeyIsEmpty() )
    {
        const Keyboard::Event e = kbd.ReadKey();
        if( e.IsRelease() )
        {
            int sel = optionSelected;
            if( e.GetCode() == VK_UP )
            {
                sel--;
                if( sel < 0 )
                {
                    sel = NUM_SELECTIONS - 1;
                }
                optionSelected = ( Selection )sel;
            }
            else if( e.GetCode() == VK_DOWN )
            {
                sel++;
                if( sel >= NUM_SELECTIONS )
                {
                    sel = 0;
                }
                optionSelected = ( Selection )sel;
            }
            else if( e.GetCode() == VK_RETURN )
            {
                if( optionSelected == START_GAME )
                {
                    startTime_enemySpawn = std::chrono::steady_clock::now();
                    gameState = GameState::PLAYING;
                    if( !gameStarted )
                    {
                        gameStarted = true;
                        ResetGame();
                    }
                }
                else if( EXIT == optionSelected )
                {
                    gameState = GameState::EXIT_GAME;
                }
                else if( DIFFICULTY == optionSelected )
                {
                    int diff = difficulty;
                    diff++;
                    diff = diff % NUM_DIFFICULTIES;
                    difficulty = ( Difficulty )diff;
                    gameStarted = false;
                    ResetGame();
                }
            }
            else if( e.GetCode() == VK_ESCAPE )
            {
                gameState = GameState::EXIT_GAME;             
            }
            else if( e.GetCode() == VK_LEFT && DIFFICULTY == optionSelected )
            {
                int diff = difficulty;
                diff--;
                if( diff < 0 )
                {
                    diff = NUM_DIFFICULTIES - 1;
                }
                difficulty = ( Difficulty )diff;
                gameStarted = false;
                ResetGame();
            }
            else if( e.GetCode() == VK_RIGHT && DIFFICULTY == optionSelected )
            {
                int diff = difficulty;
                diff++;
                diff = diff % NUM_DIFFICULTIES;
                difficulty = ( Difficulty )diff;
                gameStarted = false;
                ResetGame();
            }
        }
    }
}

void KlingKlongManager::KeyHandling( Keyboard& kbd )
{
    while( !kbd.KeyIsEmpty() )
    {
        const Keyboard::Event e = kbd.ReadKey();
        if( e.IsRelease() )
        {
            if( e.GetCode() == VK_ESCAPE )
            {
                if( GameState::GAME_OVER == gameState )
                {
                    ResetGame();
                    gameStarted = false;
                }
                gameState = GameState::START_SCREEN;
            }
            else if( e.GetCode() == 'S' )
            {
                if( GameState::STATISTICS_SCREEN == gameState && gameStarted )
                {
                    gameState = GameState::PLAYING;
                }
                else if( GameState::PLAYING == gameState )
                {
                    gameState = GameState::STATISTICS_SCREEN;
                }
            }
        }
    }

    if( kbd.KeyIsPressed( VK_RETURN ) )
    {
        ResetGame();
        gameState = GameState::PLAYING;
    }

    if( GameState::PLAYING == gameState && kbd.KeyIsPressed( VK_SPACE ) )
    {
        for( Ball& b : vBalls )
        {
            b.Start();
        }
    }
}

void KlingKlongManager::ApplyPowerUp( const PowerUp & pu )
{
    switch( pu.GetType() )
    {
    case INCR_PADDLE_SIZE:
        pad.IncreaseSize( pu.GetBoostTime() );
        stats.incrSizeCollected++;
        break;
    case EXTRA_LIFE:
        stats.extraLifeCollected++;
        if( lifes < maxLifes )
        {
            lifes++;
        } 
        break;
    case LASER_GUN:
        stats.laserGunCollected++;
        pad.AddLaserGun( pu.GetBoostTime() );
        startTime_shot = std::chrono::steady_clock::now();
        break;
    case MULTI_BALL:
        stats.multiBallCollected++;
        CreateMultiBalls();
        break;
    case SUPER_BALL:
        stats.superBallCollected++;
        vBalls[ 0 ].ActivateSuperBall( pu.GetBoostTime(), ( PowerUpSequences[ SUPER_BALL ].GetWidth() / 5 ) / 2.0f );
        break;
    default:
        break;
    }
}

void KlingKlongManager::CreateMultiBalls()
{
    if( vBalls.size() >= 3 )
    {
        //TODO maybe add possibility to get more then 3 balls later
        return;
    }
    float speed;
    switch( difficulty )
    {
    case KlingKlongManager::EASY:
        speed = 275;
        break;
    case KlingKlongManager::MEDIUM:
        speed = 500;
        break;
    case KlingKlongManager::HARD:
        speed = 625;
        break;
    case KlingKlongManager::INSANE:
        speed = 750;
        break;
    default:
        break;
    }
    while( vBalls.size() < 3 )
    {
        Ball newBall = Ball( vBalls[ 0 ].GetPosition(), Vec2(), speed, 5, 5 );
        float xDir;
        float yDir;
        do
        {
            xDir = ( float )( rand() % 200 - 100 );
            yDir = ( float )( -rand() % 100 );
        } while( abs( Paddle::maximumExitRatio * xDir ) / 2 > abs( yDir ) );
        newBall.SetDirection( Vec2( xDir, yDir ) );
        newBall.Start();

        vBalls.push_back( newBall );
    }
}

void KlingKlongManager::CreatePowerUp( const Vec2& pos, const bool enemyKilled )
{
    /* if enemy -> pos is top left, if brick -> pos is center */
    /* find free power ups -> which are currently not falling */
    int freePUs[ nPowerUps ] = { 1, 1, 1, 1, 1 };
    for( const PowerUp& p : vPowerUps )
    {
        freePUs[ p.GetType() ] = 0;
    }

    int cntFreePU = 0;
    std::vector< int > vFreeIndices;
    for( int i = 0; i < nPowerUps; ++i )
    {
        if( freePUs[ i ] )
        {
            /* check, if activation makes sense (e.g. if lifes == MAX_LIFES -> no extra life PU!) */
            if( ( vBalls.size() >= 3 && i == MULTI_BALL ) || ( i == EXTRA_LIFE && lifes == maxLifes ) )
            {
                continue;
            }
            vFreeIndices.push_back( i );
            cntFreePU++;
        }
    }
    if( 0 == cntFreePU )
    {
        /* all power ups on screen (should be really rare!) */
        return;
    }

    Vec2 posToSpawn = pos;
    if( enemyKilled )
    {
        posToSpawn += Vec2( 22, 22 ); /* current enemy size is 44x44, //TODO maybe this should be parametrized */
    }

    /* special case: only 1 free PU left and its the super ball -> 10% drop chance */
    if( 1 == cntFreePU )
    {
        if( vFreeIndices[ 0 ] == SUPER_BALL )
        {
            if( rand() % 10 == 1 )
            {
                AddPowerUp( SUPER_BALL, posToSpawn );
            }
            return;
        }
    }

    if( enemyKilled )   /* guaranteed drop, in INSANE mode only 20% */
    {
        if( INSANE == difficulty )
        {
            if( rand() % 6 < 5 )
            {
                return;
            }
        }
        int idx = rand() % cntFreePU;
        int typePU = vFreeIndices[ idx ];
        if( typePU == SUPER_BALL )
        {
            if( rand() % 10 == 1 )
            {
                AddPowerUp( SUPER_BALL, posToSpawn );
                return;
            }
            else
            {
                vFreeIndices.erase( vFreeIndices.begin() + idx );
                cntFreePU--;
                AddPowerUp( ( ePowerUpType )( rand() % cntFreePU ), posToSpawn );
                return;
            }
        }
        else
        {
            AddPowerUp( ( ePowerUpType )typePU, posToSpawn );
            return;
        }
    }
    else    /* percentage drop */
    {
        int idx = rand() % cntFreePU;
        int typePU = vFreeIndices[ idx ];

        int chance = 0;     /* the higher this value the LESS power ups will drop */
        switch( difficulty )
        {
        case KlingKlongManager::EASY:
            chance = 3;
            break;
        case KlingKlongManager::MEDIUM:
            chance = 5;
            break;
        case KlingKlongManager::HARD:
            chance = 7;
            break;
        case KlingKlongManager::INSANE:
            chance = 10;
            break;
        default:
            break;
        }

        switch( ( ePowerUpType ) typePU )
        {
        case INCR_PADDLE_SIZE:
            if( rand() % chance == 1 )
            {
                AddPowerUp( INCR_PADDLE_SIZE, posToSpawn );
            }
            break;
        case EXTRA_LIFE:
            if( rand() % chance == 1 )
            {
                AddPowerUp( EXTRA_LIFE, posToSpawn );
            }
            break;
        case LASER_GUN:
            if( rand() % chance == 1 )
            {
                AddPowerUp( LASER_GUN, posToSpawn );
            }
            break;
        case MULTI_BALL:
            if( rand() % chance == 1 )
            {
                AddPowerUp( MULTI_BALL, posToSpawn );
            }
            break;
        case SUPER_BALL:
            if( rand() % ( chance * 3 ) == 1 )
            {
                AddPowerUp( SUPER_BALL, posToSpawn );
            }
            break;
        }
    }
}

void KlingKlongManager::AddPowerUp( const ePowerUpType &type, const Vec2& posToSpawn )
{
    const float widthPU     = ( float )PowerUpSequences[ 0 ].GetWidth() / nSubImagesInSequence;
    const float heightPU    = ( float )PowerUpSequences[ 0 ].GetHeight();

    float boostTimeIncrSize = 6;
    float boostTimeLaserGun = 6;
    float boostTimeSuperBall = 4;

    switch( difficulty )
    {
    case KlingKlongManager::EASY:
        boostTimeIncrSize *= 2;
        boostTimeLaserGun *= 2;
        boostTimeSuperBall *= 2;
        break;
    case KlingKlongManager::MEDIUM:
        boostTimeIncrSize *= 1.1f;
        boostTimeLaserGun *= 1.1f;
        boostTimeSuperBall *= 1.1f;
        break;
    case KlingKlongManager::HARD:
        boostTimeIncrSize *= 0.7f;
        boostTimeLaserGun *= 0.7f;
        boostTimeSuperBall *= 0.7f;
        break;
    case KlingKlongManager::INSANE:
        boostTimeIncrSize *= 0.5f;
        boostTimeLaserGun *= 0.5f;
        boostTimeSuperBall *= 0.5f;
        break;
    default:
        break;
    }

    PowerUp PUtoSpawn;
    switch( type )
    {
    case INCR_PADDLE_SIZE:
        PUtoSpawn = PowerUp( posToSpawn, widthPU, heightPU, INCR_PADDLE_SIZE, boostTimeIncrSize, walls.GetInnerBounds().bottom, nSubImagesInSequence, 1, &soundPU_incrSize );
        break;
    case EXTRA_LIFE:
        PUtoSpawn = PowerUp( posToSpawn, widthPU, heightPU, EXTRA_LIFE, 0, walls.GetInnerBounds().bottom, nSubImagesInSequence, 1, &soundPU_incrSize );
        break;
    case LASER_GUN:
        PUtoSpawn = PowerUp( posToSpawn, widthPU, heightPU, LASER_GUN, boostTimeLaserGun, walls.GetInnerBounds().bottom, nSubImagesInSequence, 1, &soundPU_incrSize );
        break;
    case MULTI_BALL:
        PUtoSpawn = PowerUp( posToSpawn, widthPU, heightPU, MULTI_BALL, 0, walls.GetInnerBounds().bottom, nSubImagesInSequence, 1, &soundPU_incrSize );
        break;
    case SUPER_BALL:
        PUtoSpawn = PowerUp( posToSpawn, ( float )PowerUpSequences[ 4 ].GetWidth() / 5.0f, ( float )PowerUpSequences[ 4 ].GetHeight() / 5.0f,
                             SUPER_BALL, boostTimeSuperBall, walls.GetInnerBounds().bottom, 5, 5, &soundPU_superBall );
        break;
    }

    vPowerUps.push_back( PUtoSpawn );
}

void KlingKlongManager::ShootLaser()
{
    if( !pad.HasLaserGun() )
    {
        return;
    }

    const std::chrono::duration<float> timeElapsed = std::chrono::steady_clock::now() - startTime_shot;
    if( timeElapsed.count() > timeBetweenShots || !startedShooting )
    {
        soundLaserShot.Play( 1, 0.35f );
        vLaserShots.push_back( LaserShot( pad.GetRightGunPosition().GetCenter(), walls.GetInnerBounds().top ) );
        
        vLaserShots.push_back( LaserShot( pad.GetLeftGunPosition().GetCenter(), walls.GetInnerBounds().top ) );
        startTime_shot = std::chrono::steady_clock::now();

        startedShooting = true;
    }
}

void KlingKlongManager::SpawnEnemy( const Vec2& pos )
{
    if( vEnemies.size() >= maxEnemies )
    {
        return;
    }
    
    vEnemies.push_back( Enemy( pos, seqEnemy.GetWidth() / 5.0f, seqEnemy.GetHeight() / 5.0f, walls.GetInnerBounds(),
                               5, 5, EASY == difficulty || MEDIUM == difficulty ) );    
}

void KlingKlongManager::UpdateBalls( const float dt, Keyboard& kbd )
{
    auto b = vBalls.begin();
    while( b != vBalls.end() )
    {
        ( *b ).Update( dt, pad.GetRect().GetCenter().x, kbd );
        // check for enemy kills
        auto e = vEnemies.begin();
        while( e != vEnemies.end() )
        {
            if( ( *e ).CheckForCollision( ( *b ).GetRect() ) )
            {
                soundKillEnemy.Play();
                CreatePowerUp( ( *e ).GetPos(), true );

                e = vEnemies.erase( e );
                stats.enemiesKilled++;
            }
            else
            {
                e++;
            }
        }

        // wall collision check
        Ball::eBallWallColRes res = ( *b ).DoWallCollision( walls.GetInnerBounds(), pad.GetRect() );

        if( Ball::eBallWallColRes::BOTTOM_HIT == res )
        {
            b = vBalls.erase( b );
        }
        else
        {
            b++;
        }
    }

    if( vBalls.size() == 0 )
    {
        lifes--;
        startTime_enemySpawn = std::chrono::steady_clock::now();
        vPowerUps.clear();
        vLaserShots.clear();
        if( lifes > 0 )
        {
            soundLifeLoss.Play();
            ResetPaddle();
        }

        ResetBall();
    }
}

void KlingKlongManager::UpdatePaddle( const float dt, Keyboard & kbd )
{
    if( vBalls.size() > 0 )
    {
        if( vBalls[ 0 ].GetState() != WAITING )
        {
            pad.Update( kbd, dt );
        }
    }
    
    pad.DoWallCollision( walls.GetInnerBounds() );
    for( Ball& b : vBalls )
    {
        if( pad.DoBallCollision( b ) )
        {
            soundPad.Play();
        }
    }
}

void KlingKlongManager::UpdateBricks( const float dt )
{
    bool collisionHappened = false;
    float bestCollisionDistSq = 20000;  /* smaller -> better */
    int bestCollisionIdx;
    int ballIdx;

    /* if ball collides with 2+ bricks, find the one with the smallest distance to the ball */
    for( int i = 0; i < vBricks.size(); ++i )
    {
        for( int b = 0; b < vBalls.size(); ++b )
        {
            if( MOVING == vBalls[ b ].GetState() && vBricks[ i ].CheckBallCollision( vBalls[ b ] ) )
            {
                const float newColDistSq = ( vBalls[ b ].GetPosition() - vBricks[ i ].GetCenter() ).GetLengthSq();
                if( collisionHappened )
                {
                    if( newColDistSq < bestCollisionDistSq )
                    {
                        bestCollisionDistSq = newColDistSq;
                        bestCollisionIdx = i;
                        ballIdx = b;
                    }
                }
                else
                {
                    bestCollisionDistSq = newColDistSq;
                    bestCollisionIdx = i;
                    ballIdx = b;
                    collisionHappened = true;
                }
            }
        }
    }
    if( collisionHappened )
    {
        if( UNDESTROYABLE == vBricks[ bestCollisionIdx ].GetType() )
        {
            soundBrick2.Play( 1.0f, 1.5f );
        }
        else
        {
            soundBrick.Play();
        }

        if( vBricks[ bestCollisionIdx ].ExecuteBallCollision( vBalls[ ballIdx ] ) )
        {
            if( vBricks[ bestCollisionIdx ].GetType() != UNDESTROYABLE )
            {
                nBricksLeft--;
                stats.bricksDestroyed++;
                CreatePowerUp( vBricks[ bestCollisionIdx ].GetCenter(), false );
            }
            vBricks.erase( vBricks.begin() + bestCollisionIdx );
        }
    }
}

void KlingKlongManager::UpdatePowerUps( const float dt )
{
    auto it = vPowerUps.begin();
    while( it != vPowerUps.end() )
    {
        bool paddleHit = false;
        if( ( *it ).Update( pad.GetRect(), dt, paddleHit ) )
        {
            if( paddleHit )
            {
                ApplyPowerUp( *it );
            }
            it = vPowerUps.erase( it );
        }
        else
        {
            it++;
        }
    }
}

void KlingKlongManager::UpdateLaserShots( const float dt )
{
    if( pad.HasLaserGun() )
    {
        ShootLaser();
    }
    else
    {
        startedShooting = false;
    }

    auto it = vLaserShots.begin();
    while( it != vLaserShots.end() )
    {
        /* leaving screen */
        if( ( *it ).Update( dt ) )
        {
            it = vLaserShots.erase( it );
        }
        else
        {
            it++;
        }
    }

    /* enemies check */
    it = vLaserShots.begin();
    while( vEnemies.size() > 0 && it != vLaserShots.end() )
    {
        auto e = vEnemies.begin();
        bool enemyKilled = false;   /* for the enemy iterator */
        while( e != vEnemies.end() && it != vLaserShots.end() )
        {
            if( ( *e ).CheckForCollision( ( *it ).GetRect() ) )
            {
                soundKillEnemy.Play();
                CreatePowerUp( ( *e ).GetPos(), true );

                e = vEnemies.erase( e );
                it = vLaserShots.erase( it );
                enemyKilled = true;
                stats.enemiesKilled++;
            }
            else
            {
                e++;
            }
        }
        if( !enemyKilled )
        {
            it++;
        }
    }

    /* brick check */
    it = vLaserShots.begin();
    while( it != vLaserShots.end() )
    {
        auto b = vBricks.begin();
        bool laserDeleted = false;
        while( b != vBricks.end() && it != vLaserShots.end() )
        {
            bool laserHit = false;
            if( ( *b ).CheckLaserCollision( ( *it ), laserHit ) )
            {
                nBricksLeft--;
                stats.bricksDestroyed++;
                CreatePowerUp( ( *b ).GetCenter(), false );
                
                b = vBricks.erase( b );
                it = vLaserShots.erase( it );
                laserDeleted = true;
            }
            else
            {
                if( laserHit )
                {
                    it = vLaserShots.erase( it );
                    laserDeleted = true;
                }
                b++;
            }
        }
        if( !laserDeleted )
        {
            it++;
        }
    }
}

void KlingKlongManager::UpdateEnemies( const float dt )
{
    const std::chrono::duration<float> timeElapsed = std::chrono::steady_clock::now() - startTime_enemySpawn;
    if( timeElapsed.count() > timeBetweenEnemies )
    {
        SpawnEnemy();
        startTime_enemySpawn = std::chrono::steady_clock::now();
    }

    auto e = vEnemies.begin();
    while( e != vEnemies.end() )
    {
        ( *e ).Update( dt );

        if( ( *e ).CheckForCollision( pad.GetRect() ) )
        {
            lifes--;
            startTime_enemySpawn = std::chrono::steady_clock::now();
            if( lifes > 0 )
            {
                soundEnemyHit.Play();
            }
            e = vEnemies.erase( e );
        }
        else
        {
            e++;
        }
    }
}

void KlingKlongManager::ResetBall()
{
    vBalls.clear();
    float speed;
    switch( difficulty )
    {
    case KlingKlongManager::EASY:
        speed = 275;
        break;
    case KlingKlongManager::MEDIUM:
        speed = 400;
        break;
    case KlingKlongManager::HARD:
        speed = 600;
        break;
    case KlingKlongManager::INSANE:
        speed = 750;
        break;
    default:
        break;
    }
    vBalls.push_back( Ball( Vec2( pad.GetRect().GetCenter().x, pad.GetRect().top - 7 ), Vec2( -0.2f, -1 ), speed, 5, 5 ) );
}

void KlingKlongManager::ResetPaddle()
{
    pad = Paddle( Vec2( gfx.ScreenWidth / 2, gfx.ScreenHeight - 110 ), EASY == difficulty );
}

void KlingKlongManager::DrawScene()
{
    switch( gameState )
    {
    case GameState::START_SCREEN:
    {
        int x = gfx.ScreenWidth / 2 - sur_WelcomeTo.GetWidth() / 2;
        int y = gfx.ScreenHeight / 5;
        gfx.DrawSpriteKey( x, y, sur_WelcomeTo, sur_WelcomeTo.GetPixel( 0, 0 ) );
        x = gfx.ScreenWidth / 2 - sur_KlingKlong.GetWidth() / 2;
        y = gfx.ScreenHeight / 3;
        if( 1 == startScreenCnt )
        {
            gfx.DrawSpriteKeyFromSequence( x, y, sur_KlingKlong, sur_KlingKlong.GetPixel( 0, 0 ), 0, 2, 1 );
        }
        else if( startScreenCnt >= 2 )
        {
            gfx.DrawSpriteKey( x, y, sur_KlingKlong, sur_KlingKlong.GetPixel( 0, 0 ) );
            if( 3 == startScreenCnt )
            {
                y = gfx.ScreenHeight / 2 + 30;
                if( gameStarted )
                {
                    x = gfx.ScreenWidth / 2 - sur_Continue.GetWidth() / 2;
                    gfx.DrawSpriteKey( x, y, sur_Continue, sur_Continue.GetPixel( 0, 0 ) );
                }
                else
                {
                    x = gfx.ScreenWidth / 2 - sur_StartGame.GetWidth() / 2;
                    gfx.DrawSpriteKey( x, y, sur_StartGame, sur_StartGame.GetPixel( 0, 0 ) );
                }
                if( Selection::START_GAME == optionSelected )
                {
                    if( gameStarted )
                    {
                        gfx.DrawRectBorder( RectF( Vec2( ( float )x, ( float )y ),
                            ( float )sur_Continue.GetWidth(), ( float )sur_Continue.GetHeight() ), 1, Colors::LightGray );
                    }
                    else
                    {
                        gfx.DrawRectBorder( RectF( Vec2( ( float )x, ( float )y ),
                            ( float )sur_StartGame.GetWidth(), ( float )sur_StartGame.GetHeight() ), 1, Colors::LightGray );
                    }
                }
                y += 50;
                if( EASY == difficulty )
                {
                    x = gfx.ScreenWidth / 2 - sur_Difficulty.GetWidth() / 2 - sur_Easy.GetWidth() / 2;
                    gfx.DrawSpriteKey( x, y, sur_Difficulty, sur_Difficulty.GetPixel( 0, 0 ) );
                    if( Selection::DIFFICULTY == optionSelected )
                    {
                        gfx.DrawRectBorder( RectF( Vec2( ( float )x, ( float )y ),
                            ( float )sur_Difficulty.GetWidth() + sur_Easy.GetWidth(), ( float )sur_Difficulty.GetHeight() - 6 ), 1, Colors::Green );
                    }
                    x += sur_Difficulty.GetWidth();
                    gfx.DrawSpriteKey( x, y + 6, sur_Easy, sur_Easy.GetPixel( 0, 0 ) );
                }
                else if( MEDIUM == difficulty )
                {
                    x = gfx.ScreenWidth / 2 - sur_Difficulty.GetWidth() / 2 - sur_Medium.GetWidth() / 2;
                    gfx.DrawSpriteKey( x, y, sur_Difficulty, sur_Difficulty.GetPixel( 0, 0 ) );
                    if( Selection::DIFFICULTY == optionSelected )
                    {
                        gfx.DrawRectBorder( RectF( Vec2( ( float )x, ( float )y ),
                            ( float )sur_Difficulty.GetWidth() + sur_Medium.GetWidth(), ( float )sur_Difficulty.GetHeight() - 6 ), 1, Colors::Yellow );
                    }
                    x += sur_Difficulty.GetWidth();
                    gfx.DrawSpriteKey( x, y, sur_Medium, sur_Medium.GetPixel( 0, 0 ) );
                }
                else if( HARD == difficulty )
                {
                    x = gfx.ScreenWidth / 2 - sur_Difficulty.GetWidth() / 2 - sur_Hard.GetWidth() / 2;
                    gfx.DrawSpriteKey( x, y, sur_Difficulty, sur_Difficulty.GetPixel( 0, 0 ) );
                    if( Selection::DIFFICULTY == optionSelected )
                    {
                        gfx.DrawRectBorder( RectF( Vec2( ( float )x, ( float )y ),
                            ( float )sur_Difficulty.GetWidth() + sur_Hard.GetWidth(), ( float )sur_Difficulty.GetHeight() - 6 ), 1, Colors::Red );
                    }
                    x += sur_Difficulty.GetWidth();
                    gfx.DrawSpriteKey( x, y, sur_Hard, sur_Hard.GetPixel( 0, 0 ) );
                }
                else if( INSANE == difficulty )
                {
                    x = gfx.ScreenWidth / 2 - sur_Difficulty.GetWidth() / 2 - sur_Insane.GetWidth() / 2;
                    gfx.DrawSpriteKey( x, y, sur_Difficulty, sur_Difficulty.GetPixel( 0, 0 ) );
                    if( Selection::DIFFICULTY == optionSelected )
                    {
                        gfx.DrawRectBorder( RectF( Vec2( ( float )x, ( float )y ),
                            ( float )sur_Difficulty.GetWidth() + sur_Insane.GetWidth(), ( float )sur_Difficulty.GetHeight() - 6 ), 3, Colors::Magenta );
                    }
                    x += sur_Difficulty.GetWidth();
                    gfx.DrawSpriteKey( x, y, sur_Insane, sur_Insane.GetPixel( 0, 0 ) );
                }

                x = gfx.ScreenWidth / 2 - sur_Exit.GetWidth() / 2;
                y += 50;
                gfx.DrawSpriteKey( x, y, sur_Exit, sur_Exit.GetPixel( 0, 0 ) );
                if( Selection::EXIT == optionSelected )
                {
                    gfx.DrawRectBorder( RectF( Vec2( ( float )x, ( float )y ),
                        ( float )sur_Exit.GetWidth(), ( float )sur_Exit.GetHeight() ), 1, Colors::LightGray );
                }
            }
        }
    }
    break;
    case GameState::PLAYING:
    {
#if !_DEBUG
        gfx.DrawSprite( 0, gfx.ScreenHeight - sur_Background.GetHeight(), sur_Background );
#endif
        walls.Draw( gfx );
        for( const Ball& b : vBalls )
        {
            b.Draw( gfx, PowerUpSequences[ SUPER_BALL ], EASY == difficulty );
        }
        for( const Brick& b : vBricks )
        {
            b.Draw( gfx );
        }
        for( const PowerUp& p : vPowerUps )
        {
            p.Draw( gfx, PowerUpSequences[ p.GetType() ] );
        }
        for( const LaserShot& l : vLaserShots )
        {
            l.Draw( gfx );
        }
        for( const Enemy& e : vEnemies )
        {
            e.Draw( gfx, seqEnemy );
        }

        pad.Draw( gfx );
        pad.DrawAsLifesRemaining( gfx, lifes, Vec2( walls.GetInnerBounds().left, 15 ), 0.3f );

#if !_DEBUG
        DrawLightning();
#else
        gfx.DrawString( std::string( "Enemies: " + std::to_string( vEnemies.size() ) ).c_str(), 400, gfx.ScreenHeight - 30, font, fontSurface, Colors::White );
        //gfx.DrawString( std::to_string( vLaserShots.size() ).c_str(), 600, gfx.ScreenHeight - 30, font, fontSurface, Colors::White );
        gfx.DrawString( std::string( "Bricks left: " + std::to_string( nBricksLeft ) ).c_str(), 600, gfx.ScreenHeight - 30, font, fontSurface, Colors::White );
#endif

        std::string lvlTxt = "Level " + std::to_string( level + 1 );
        gfx.DrawString( lvlTxt.c_str(), 700, 10, font, fontSurface, Colors::White );
        switch( difficulty )
        {
        case KlingKlongManager::EASY:
            gfx.DrawString( "Easy", gfx.ScreenWidth / 2 - 30, 10, font, fontSurface, Colors::Green );
            break;
        case KlingKlongManager::MEDIUM:
            gfx.DrawString( "Medium", gfx.ScreenWidth / 2 - 30, 10, font, fontSurface, Colors::Yellow );
            break;
        case KlingKlongManager::HARD:
            gfx.DrawString( "Hard", gfx.ScreenWidth / 2 - 30, 10, font, fontSurface, Colors::Red );
            break;
        case KlingKlongManager::INSANE:
            gfx.DrawString( "INSANE", gfx.ScreenWidth / 2 - 30, 10, font, fontSurface, Colors::Magenta );
            break;
        default:
            break;
        }
    }
    break;
    case GameState::VICTORY_SCREEN:
    {
#if !_DEBUG
        gfx.DrawSprite( 0, gfx.ScreenHeight - sur_Background.GetHeight(), sur_Background );
#endif
        walls.Draw( gfx );
        pad.Draw( gfx );
        for( const Ball& b : vBalls )
        {
            b.Draw( gfx, PowerUpSequences[ SUPER_BALL ] );
        }
        for( const Brick& b : vBricks )
        {
            b.Draw( gfx );
        }
        for( const PowerUp& p : vPowerUps )
        {
            p.Draw( gfx, PowerUpSequences[ p.GetType() ] );
        }
        for( const LaserShot& l : vLaserShots )
        {
            l.Draw( gfx );
        }
        for( const Enemy& e : vEnemies )
        {
            e.Draw( gfx, seqEnemy );
        }
        gfx.DrawSpriteKey( ( int )walls.GetInnerBounds().GetCenter().x - sur_Victory.GetWidth() / 2, 200, sur_Victory, sur_Victory.GetPixel( 0, 0 ) );
    }
    break;
    case GameState::STATISTICS_SCREEN:
    {
        DrawStatisticsScreen();
    }
    break;
    case GameState::GAME_OVER:
    {
#if !_DEBUG
        gfx.DrawSprite( 0, gfx.ScreenHeight - sur_Background.GetHeight(), sur_Background );
        DrawLightning();
#endif
        for( const Brick& b : vBricks )
        {
            b.Draw( gfx );
        }
        if( explSeqIdx > 3 )
        {
            const int x = ( Graphics::ScreenWidth - sur_GameOver.GetWidth() ) / 2;
            const int y = ( Graphics::ScreenHeight - sur_GameOver.GetHeight() ) / 3;
            gfx.DrawSpriteKey( x, y, sur_GameOver, sur_GameOver.GetPixel( 0, 0 ) );
        }
        else
        {
            const float x = pad.GetRect().GetCenter().x - PadExplosion[ explSeqIdx ].GetWidth() / 2;
            const float y = pad.GetRect().GetCenter().y - PadExplosion[ explSeqIdx ].GetHeight() / 2;
            gfx.DrawSpriteKey( ( int )x, ( int )y, PadExplosion[ explSeqIdx ], PadExplosion[ explSeqIdx ].GetPixel( 0, 0 ) );
        }
    }
    break;

    default:
        break;
    }
}

void KlingKlongManager::CreateNextLevel()
{
    if( level > 6 )
    {
        level = 0;
    }
    /* clear all bricks first */
    vBricks.clear();

    float fieldWidth    = walls.GetInnerBounds().right - walls.GetInnerBounds().left;
    int nBricksAcross   = 12;
    int nBricksDown     = 12;

    float brickWidth    = fieldWidth / nBricksAcross;
    float brickHeight   = 22.0f;
    const Vec2 topLeft( walls.GetInnerBounds().left + 1, walls.GetInnerBounds().top + 1 );

    nBricksLeft = 0;
    
    const int startRow = level * ( nBricksDown + 1 ) + 1;
    float y = 0;
    for( size_t r = startRow; r < startRow + nBricksDown; r++ )
    {
        for( size_t c = 0; c < nBricksAcross; c++ )
        {
            switch( allLevels[ r ][ c ] )
            {
            case 'R':   // solid red brick (2 life)
            {
                vBricks.push_back( Brick( RectF( topLeft + Vec2( c * brickWidth, y * brickHeight ), brickWidth, brickHeight ), { 230, 0, 0 }, SOLID, 2 ) );
                nBricksLeft++;
            }
            break;
            case 'r':   // red brick
            {
                vBricks.push_back( Brick( RectF( topLeft + Vec2( c * brickWidth, y * brickHeight ), brickWidth, brickHeight ), { 230, 0, 0 } ) );
                nBricksLeft++;
            }
            break;
            case 'G':   // solid green brick (2 life)
            {
                vBricks.push_back( Brick( RectF( topLeft + Vec2( c * brickWidth, y * brickHeight ), brickWidth, brickHeight ), { 0, 230, 0 }, SOLID, 2 ) );
                nBricksLeft++;
            }
            break;
            case 'g':   // green brick
            {
                vBricks.push_back( Brick( RectF( topLeft + Vec2( c * brickWidth, y * brickHeight ), brickWidth, brickHeight ), { 0, 230, 0 } ) );
                nBricksLeft++;
            }
            break;
            case 'B':   // solid blue brick (2 life)
            {
                vBricks.push_back( Brick( RectF( topLeft + Vec2( c * brickWidth, y * brickHeight ), brickWidth, brickHeight ), { 0, 0, 230 }, SOLID, 2 ) );
                nBricksLeft++;
            }
            break;
            case 'b':   // blue brick
            {
                vBricks.push_back( Brick( RectF( topLeft + Vec2( c * brickWidth, y * brickHeight ), brickWidth, brickHeight ), { 0, 0, 230 } ) );
                nBricksLeft++;
            }
            break;
            case 'Y':   // solid yellow brick (2 life)
            {
                vBricks.push_back( Brick( RectF( topLeft + Vec2( c * brickWidth, y * brickHeight ), brickWidth, brickHeight ), { 230, 230, 0 }, SOLID, 2 ) );
                nBricksLeft++;
            }
            break;
            case 'y':   // yellow brick
            {
                vBricks.push_back( Brick( RectF( topLeft + Vec2( c * brickWidth, y * brickHeight ), brickWidth, brickHeight ), { 230, 230, 0 } ) );
                nBricksLeft++;
            }
            break;
            case 'C':   // solid cyan brick (2 life)
            {
                vBricks.push_back( Brick( RectF( topLeft + Vec2( c * brickWidth, y * brickHeight ), brickWidth, brickHeight ), { 0, 230, 230 }, SOLID, 2 ) );
                nBricksLeft++;
            }
            break;
            case 'c':   // cyan brick
            {
                vBricks.push_back( Brick( RectF( topLeft + Vec2( c * brickWidth, y * brickHeight ), brickWidth, brickHeight ), { 0, 230, 230 } ) );
                nBricksLeft++;
            }
            break;
            case 'U':   // undestroyable brick
            {
                vBricks.push_back( Brick( RectF( topLeft + Vec2( c * brickWidth, y * brickHeight ), brickWidth, brickHeight ), Colors::LightGray, UNDESTROYABLE ) );
            }
            break;
            default:
                break;
            };
        }
        y++;
    }
}

void KlingKlongManager::DrawLightning()
{
    const int subWidth      = seqLightning.GetWidth() / nSubImagesInSequence;
    const int subImgInLevel = ( int )( walls.GetInnerBounds().right - walls.GetInnerBounds().left ) / subWidth;
    for( int i = 0; i < 19; ++i )
    {
        gfx.DrawSpriteKeyFromSequence( ( int )walls.GetInnerBounds().left + 5 + subWidth * ( ( lightningSeqIdx + i ) % 19 ), gfx.ScreenHeight - 80,
                                       seqLightning, seqLightning.GetPixel( 0, 0 ), i % 8, 8, 1 );
    }

    const std::chrono::duration<float> timeElapsed = std::chrono::steady_clock::now() - startTime_lightning;
    if( timeElapsed.count() >= 0.1 )
    {
        lightningSeqIdx++;
        if( lightningSeqIdx >= nSubImagesInSequence )
        {
            lightningSeqIdx = 0;
        }
        startTime_lightning = std::chrono::steady_clock::now();
    }
}

void KlingKlongManager::DrawStatisticsScreen()
{
#if _DEBUG
    gfx.DrawSprite( 0, gfx.ScreenHeight - sur_Background.GetHeight(), sur_Background );
#endif
    gfx.DrawSprite( gfx.ScreenWidth / 2 - sur_Statistics.GetWidth() / 2, 100, sur_Statistics );

    int x = 350;
    int y = 330;
    gfx.DrawSpriteKeyFromSequence( x, y, seqEnemy, seqEnemy.GetPixel( 0, 0 ), 0, 5, 5 );
    gfx.DrawString( std::string( "x" + std::to_string( stats.enemiesKilled ) ).c_str(), x + 150, y, font, fontSurface, Colors::Green );
    y += 50;
    gfx.DrawSpriteKeyFromSequence( x, y + 5, PowerUpSequences[ INCR_PADDLE_SIZE ], PowerUpSequences[ INCR_PADDLE_SIZE ].GetPixel( 0, 0 ), 0, nSubImagesInSequence, 1 );
    gfx.DrawString( std::string( "x" + std::to_string( stats.incrSizeCollected ) ).c_str(), x + 150, y, font, fontSurface, Colors::Green );
    y += 50;
    gfx.DrawSpriteKeyFromSequence( x, y, PowerUpSequences[ EXTRA_LIFE ], PowerUpSequences[ EXTRA_LIFE ].GetPixel( 0, 0 ), 0, nSubImagesInSequence, 1 );
    gfx.DrawString( std::string( "x" + std::to_string( stats.extraLifeCollected ) ).c_str(), x + 150, y, font, fontSurface, Colors::Green );
    y += 50;
    gfx.DrawSpriteKeyFromSequence( x, y - 15, PowerUpSequences[ LASER_GUN ], PowerUpSequences[ LASER_GUN ].GetPixel( 0, 0 ), 0, nSubImagesInSequence, 1 );
    gfx.DrawString( std::string( "x" + std::to_string( stats.laserGunCollected ) ).c_str(), x + 150, y, font, fontSurface, Colors::Green );
    y += 50;
    gfx.DrawSpriteKeyFromSequence( x, y, PowerUpSequences[ MULTI_BALL ], PowerUpSequences[ MULTI_BALL ].GetPixel( 0, 0 ), 0, nSubImagesInSequence, 1 );
    gfx.DrawString( std::string( "x" + std::to_string( stats.multiBallCollected ) ).c_str(), x + 150, y, font, fontSurface, Colors::Green );
    y += 50;
    gfx.DrawSpriteKeyFromSequence( x, y - 5, PowerUpSequences[ SUPER_BALL ], PowerUpSequences[ SUPER_BALL ].GetPixel( 0, 0 ), 0, 5, 5 );
    gfx.DrawString( std::string( "x" + std::to_string( stats.superBallCollected ) ).c_str(), x + 150, y, font, fontSurface, Colors::Green );
    y += 50;

    Beveler bev( { 225, 10, 5 } );
    RectF rect( Vec2( ( float )x, ( float )y ), 70, 30 );
    bev.DrawBeveledBrick( rect.GetExpanded( -2.5 ), 5, gfx );
    gfx.DrawString( std::string( "x" + std::to_string( stats.bricksDestroyed ) ).c_str(), x + 150, y, font, fontSurface, Colors::Green );
}
