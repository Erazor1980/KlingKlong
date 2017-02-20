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

KlingKlongManager::KlingKlongManager( Graphics& gfx_in, GameState& gameState_in, const Walls& walls_in, Paddle& pad_in )
    :
    gfx( gfx_in ),
    gameState( gameState_in ),
    walls( walls_in ),
    pad( pad_in )
{
    startScreenCnt = 3;

    allLevels = LoadTextFile( "level.txt" );

    ResetGame();
}

void KlingKlongManager::ResetGame()
{
    // seed rand()
    std::srand( ( unsigned int )std::time( 0 ) );

    // level
    level = 4;

    // reset paddle
    ResetPaddle();

    // reset ball
    ResetBall();

    // reset bricks
    CreateNextLevel();

    // reset life
    lifes = MAX_LIFES;

    // reset power ups
    vPowerUps.clear();

    // reset shots
    vLaserShots.clear();

    // reset enemies
    vEnemies.clear();

    // explosion sequence
    explSeqIdx = 0;

    // reset timer
    startTime_shot          = std::chrono::steady_clock::now();
    startTime_enemySpawn    = std::chrono::steady_clock::now();

    /////////////////
    //// TESTING ////
    /////////////////
    //AddPowerUp( INCR_PADDLE_SIZE, Vec2( 450, 100 ) );
    //AddPowerUp( EXTRA_LIFE, Vec2( 450, 100 ) );
    //AddPowerUp( LASER_GUN, Vec2( 450, 100 ) );
    //AddPowerUp( MULTI_BALL, Vec2( 450, 100 ) );
    //AddPowerUp( SUPER_BALL, Vec2( 450, 100 ) );
    //vBalls[ 0 ].StickToPaddle( pad.GetRect().GetCenter().x );
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

        if( 0 == lifes )
        {
            soundGameOver.Play();
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
        const std::chrono::duration<float> timeElapsed = std::chrono::steady_clock::now() - startTime_explosion;
        if( timeElapsed.count() > 0.15f )
        {
            explSeqIdx++;
            startTime_explosion = std::chrono::steady_clock::now();
        }
        if( explSeqIdx >= 4 )
        {
            KeyHandling( kbd );
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
            gameState = GameState::PLAYING;
        }
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
                    gameState = GameState::PLAYING;
                }
                else if( EXIT == optionSelected )
                {
                    gameState = GameState::EXIT_GAME;
                }
            }
            else if( e.GetCode() == VK_ESCAPE )
            {
                gameState = GameState::EXIT_GAME;             
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
                }
                gameState = GameState::START_SCREEN;
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
        break;
    case EXTRA_LIFE:
        if( lifes < MAX_LIFES )
        {
            lifes++;
        }
        break;
    case LASER_GUN:
        pad.AddLaserGun( pu.GetBoostTime() );
        startTime_shot = std::chrono::steady_clock::now();
        break;
    case MULTI_BALL:
        CreateMultiBalls();
        break;
    case SUPER_BALL:
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

    while( vBalls.size() < 3 )
    {
        Ball newBall = Ball( vBalls[ 0 ].GetPosition(), Vec2(), 5, 5 );
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
            if( ( vBalls.size() >= 3 && i == MULTI_BALL ) || ( i == EXTRA_LIFE && lifes == MAX_LIFES ) )
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
                return;
            }
        }
    }

    if( enemyKilled )   /* guaranteed drop */
    {
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
        switch( ( ePowerUpType ) typePU )
        {
        case INCR_PADDLE_SIZE:
            if( rand() % 5 == 1 )
            {
                AddPowerUp( INCR_PADDLE_SIZE, posToSpawn );
            }
            break;
        case EXTRA_LIFE:
            if( rand() % 5 == 1 )
            {
                AddPowerUp( EXTRA_LIFE, posToSpawn );
            }
            break;
        case LASER_GUN:
            if( rand() % 5 == 1 )
            {
                AddPowerUp( LASER_GUN, posToSpawn );
            }
            break;
        case MULTI_BALL:
            if( rand() % 5 == 1 )
            {
                AddPowerUp( MULTI_BALL, posToSpawn );
            }
            break;
        case SUPER_BALL:
            if( rand() % 15 == 1 )
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

    float boostTimeIncrSize = 5;
    float boostTimeLaserGun = 4;
#if EASY_MODE
    boostTimeIncrSize *= 2;
    boostTimeLaserGun *= 2;
#endif

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
                             SUPER_BALL, 5, walls.GetInnerBounds().bottom, 5, 5, &soundPU_superBall );
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
    if( vEnemies.size() >= MAX_ENEMIES )
    {
        return;
    }

    vEnemies.push_back( Enemy( pos, seqEnemy.GetWidth() / 5.0f, seqEnemy.GetHeight() / 5.0f, walls.GetInnerBounds(), 5, 5 ) );
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
        ResetPaddle();
        vPowerUps.clear();
        vLaserShots.clear();
        soundLifeLoss.Play();

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
        bool brickHit = false;
        while( b != vBricks.end() && it != vLaserShots.end() )
        {
            if( ( *b ).CheckLaserCollision( ( *it ) ) )
            {
                nBricksLeft--;
                CreatePowerUp( ( *b ).GetCenter(), false );
                
                b = vBricks.erase( b );
                it = vLaserShots.erase( it );
                brickHit = true;
            }
            else
            {
                b++;
            }
        }
        if( !brickHit )
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
    vBalls.push_back( Ball( Vec2( pad.GetRect().GetCenter().x, pad.GetRect().top - 7 ), Vec2( -0.2f, -1 ), 5, 5 ) );
}

void KlingKlongManager::ResetPaddle()
{
    pad = Paddle( Vec2( gfx.ScreenWidth / 2, gfx.ScreenHeight - 80 ) );
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
                x = gfx.ScreenWidth / 2 - sur_StartGame.GetWidth() / 2;
                y = gfx.ScreenHeight / 2 + 30;
                gfx.DrawSpriteKey( x, y, sur_StartGame, sur_StartGame.GetPixel( 0, 0 ) );
                if( Selection::START_GAME == optionSelected )
                {
                    gfx.DrawRectBorder( RectF( Vec2( ( float )x, ( float )y ),
                        ( float )sur_StartGame.GetWidth(), ( float )sur_StartGame.GetHeight() ), 1, Colors::LightGray );
                }
                //TODO difficulties not implemented yet
                /*x = gfx.ScreenWidth / 2 - sur_Difficulty.GetWidth() / 2;
                y += 50;
                gfx.DrawSpriteKey( x, y, sur_Difficulty, sur_Difficulty.GetPixel( 0, 0 ) );*/
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
        gfx.DrawSprite( 0, 0, sur_Background );
#endif
        walls.Draw( gfx );
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

        pad.Draw( gfx );
        pad.DrawAsLifesRemaining( gfx, lifes, Vec2( walls.GetInnerBounds().left, 15 ), 0.3f );

#if _DEBUG
        gfx.DrawString( std::to_string( vEnemies.size() ).c_str(), 400, gfx.ScreenHeight - 30, font, fontSurface, Colors::White );
        gfx.DrawString( std::to_string( vLaserShots.size() ).c_str(), 600, gfx.ScreenHeight - 30, font, fontSurface, Colors::White );
        gfx.DrawString( std::to_string( nBricksLeft ).c_str(), 800, gfx.ScreenHeight - 30, font, fontSurface, Colors::White );
#endif

        std::string lvlTxt = "Level " + std::to_string( level + 1 );
        gfx.DrawString( lvlTxt.c_str(), 700, 10, font, fontSurface, Colors::White );
    }
    break;
    case GameState::VICTORY_SCREEN:
    {
        walls.Draw( gfx );
        pad.Draw( gfx );
        gfx.DrawSpriteKey( ( int )walls.GetInnerBounds().GetCenter().x - sur_Victory.GetWidth() / 2, 200, sur_Victory, sur_Victory.GetPixel( 0, 0 ) );
    }
    break;
    case GameState::GAME_OVER:
    {
        for( const Brick& b : vBricks )
        {
            b.Draw( gfx );
        }
        if( explSeqIdx > 3 )
        {
            const int x = ( Graphics::ScreenWidth - sur_GameOver.GetWidth() ) / 2;
            const int y = ( Graphics::ScreenHeight - sur_GameOver.GetHeight() ) / 2;
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
    if( level > 4 )
    {
        level = 0;
    }
    /* clear all bricks first */
    vBricks.clear();

    float fieldWidth    = walls.GetInnerBounds().right - walls.GetInnerBounds().left;
    int nBricksAcross   = 12;
    int nBricksDown     = 10;

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