#include "KlingKlongManager.h"
#include <ctime>

KlingKlongManager::KlingKlongManager( Graphics& gfx_in, GameState& gameState_in, const Walls& walls_in, Paddle& pad_in )
                                      //,std::vector< Ball >& vBalls_in, std::vector< Brick >& vBricks_in, std::vector< PowerUp >& vPowerUps_in )
    :
    gfx( gfx_in ),
    gameState( gameState_in ),
    walls( walls_in ),
    pad( pad_in )
    /*,vBalls( vBalls_in ),
    vBricks( vBricks_in ),
    vPowerUps( vPowerUps_in )    */
{
    startScreenCnt = 3;

    ResetGame();
}

void KlingKlongManager::ResetGame()
{
    // seed rand()
    std::srand( ( unsigned int )std::time( 0 ) );

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


    /////////////////
    //// TESTING ////
    /////////////////
    const float widthPU     = ( float )PowerUpSequences[ 0 ].GetWidth() / nSubImagesInSequence;
    const float heightPU    = ( float )PowerUpSequences[ 0 ].GetHeight();

    float boostTimeIncrSize = 5;
    float boostTimeLaserGun = 4;
#if EASY_MODE
    boostTimeIncrSize *= 2;
    boostTimeLaserGun *= 2;
#endif
    //vPowerUps.push_back( PowerUp( Vec2( 300, 100 ), widthPU, heightPU, INCR_PADDLE_SIZE, boostTimeIncrSize, walls.GetInnerBounds().bottom, nSubImagesInSequence, 1, &soundPU_incrSize ) );
    //vPowerUps.push_back( PowerUp( Vec2( 450, 100 ), widthPU, heightPU, EXTRA_LIFE, 0, walls.GetInnerBounds().bottom, nSubImagesInSequence, 1, &soundPU_extraLife ) );
    vPowerUps.push_back( PowerUp( Vec2( 450, 100 ), widthPU, heightPU, LASER_GUN, boostTimeLaserGun, walls.GetInnerBounds().bottom, nSubImagesInSequence, 1, NULL ) );
    //vPowerUps.push_back( PowerUp( Vec2( 450, 100 ), widthPU, heightPU, MULTI_BALL, 0, walls.GetInnerBounds().bottom, nSubImagesInSequence, 1, &soundPU_multiBall ) );
    //vPowerUps.push_back( PowerUp( Vec2( 450, 100 ), ( float )PowerUpSequences[ 4 ].GetWidth() / 5.0f, ( float )PowerUpSequences[ 4 ].GetHeight() / 5.0f, SUPER_BALL, 5, walls.GetInnerBounds().bottom, 5, 5, &soundPU_superBall ) );
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
    }
    default:
        break;
    }
}

void KlingKlongManager::UpdateStartScreen( Keyboard& kbd )
{
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
                gameState = GameState::START_SCREEN;
            }
        }
    }

    if( kbd.KeyIsPressed( VK_SPACE ) )
    {
        for( Ball& b : vBalls )
        {
            b.Start();
        }
    }

    if( kbd.KeyIsPressed( VK_RETURN ) )
    {
        ResetGame();
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

void KlingKlongManager::UpdateBalls( const float dt, Keyboard & kbd )
{
    for( Ball& b : vBalls )
    {
        b.Update( dt, 200, kbd );
        b.DoWallCollision( walls.GetInnerBounds(), pad.GetRect() );

        //TODO
        //enemy collision test!
    }
}

void KlingKlongManager::UpdatePaddle( const float dt, Keyboard & kbd )
{
    //if( balls[ lastBallIdx ].GetState() != WAITING )
    if( vBalls.size() > 0 )
    {
        pad.Update( kbd, dt );
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

        pad.Draw( gfx );

        gfx.DrawString( std::to_string( nBricksLeft ).c_str(), 800, 10, font, fontSurface, Colors::White );

        gfx.DrawString( std::to_string( vLaserShots.size() ).c_str(), 600, 10, font, fontSurface, Colors::White );
    }
    break;
    case GameState::VICTORY_SCREEN:
        break;
    case GameState::GAME_OVER:
        break;

    default:
        break;
    }
}

void KlingKlongManager::CreateNextLevel()
{
    if( level > 2 )
    {
        level = 0;
    }

    /* clear all bricks first */
    vBricks.clear();

    float fieldWidth    = walls.GetInnerBounds().right - walls.GetInnerBounds().left;
    int nBricksAcross   = 14;
    int nBricksDown     = 4;

    float brickWidth    = fieldWidth / nBricksAcross;
    float brickHeight   = 22.0f;

    const Color brickColors[ 4 ] = { { 230, 0, 0 },{ 0, 230, 0 },{ 0, 0, 230 },{ 0, 230 ,230 } };

    const Vec2 topLeft( walls.GetInnerBounds().left + 1, walls.GetInnerBounds().top + 1 );
    if( 0 == level )
    {
        nBricksLeft = 0;
        for( int y = 0; y < nBricksDown; ++y )
        {
            const Color c = brickColors[ y ];
            for( int x = 0; x < nBricksAcross; ++x )
            {
                if( 0 == x || nBricksAcross - 1 == x )
                {
                    vBricks.push_back( Brick( RectF( topLeft + Vec2( x * brickWidth, y * brickHeight ),
                                                                    brickWidth, brickHeight ), Colors::LightGray, UNDESTROYABLE ) );
                }
                else
                {
                    if( 0 == y && x % 2 )
                    {
                        vBricks.push_back( Brick( RectF( topLeft + Vec2( x * brickWidth, y * brickHeight ), brickWidth, brickHeight ), c, SOLID, 2 ) );
                        nBricksLeft++;
                    }
                    else
                    {
                        vBricks.push_back( Brick( RectF( topLeft + Vec2( x * brickWidth, y * brickHeight ), brickWidth, brickHeight ), c ) );
                        nBricksLeft++;
                    }
                }                
            }
        }
    }
    //else if( 1 == level )
    //{
    //    nBricksLeft = 0;
    //    for( int y = 0; y < nBricksDown; ++y )
    //    {
    //        const Color c = brickColors[ 3 - y ];
    //        for( int x = 0; x < nBricksAcross; ++x )
    //        {
    //            if( y % 2 - 1 && x % 2 - 1 )
    //            {
    //                // empty space
    //            }
    //            else
    //            {
    //                if( ( 0 == y || 2 == y ) && x % 2 )
    //                {
    //                    bricks[ y * nBricksAcross + x ] = Brick( RectF( topLeft + Vec2( x * brickWidth, y * brickHeight ),
    //                                                                    brickWidth, brickHeight ), c, SOLID, 2 );
    //                    nBricksLeft++;
    //                }
    //                else
    //                {
    //                    bricks[ y * nBricksAcross + x ] = Brick( RectF( topLeft + Vec2( x * brickWidth, y * brickHeight ), brickWidth, brickHeight ), c );
    //                    nBricksLeft++;
    //                }
    //            }
    //        }
    //    }

    //    bricks[ 0 ] = Brick( RectF( topLeft + Vec2( -brickWidth, ( nBricksDown - 1 ) * brickHeight ),
    //                                brickWidth, brickHeight ), Colors::LightGray, UNDESTROYABLE );
    //    bricks[ 2 ] = Brick( RectF( topLeft + Vec2( nBricksAcross * brickWidth, ( nBricksDown - 1 ) * brickHeight ),
    //                                brickWidth, brickHeight ), Colors::LightGray, UNDESTROYABLE );
    //}
    //else if( 2 == level )
    //{
    //    nBricksLeft = 0;
    //    for( int y = 0; y < nBricksDown; ++y )
    //    {
    //        const Color c = brickColors[ 3 - y ];
    //        for( int x = 0; x < nBricksAcross; ++x )
    //        {
    //            if( 3 == y && ( x < 3 || x > 9 ) )
    //            {
    //                bricks[ y * nBricksAcross + x ] = Brick( RectF( topLeft + Vec2( x * brickWidth, y * brickHeight ),
    //                                                                brickWidth, brickHeight ), Colors::LightGray, UNDESTROYABLE );
    //            }
    //            else
    //            {
    //                if( ( 0 == y || 2 == y ) && x % 2 )
    //                {
    //                    bricks[ y * nBricksAcross + x ] = Brick( RectF( topLeft + Vec2( x * brickWidth, y * brickHeight ),
    //                                                                    brickWidth, brickHeight ), c, SOLID, 2 );
    //                    nBricksLeft++;
    //                }
    //                else
    //                {
    //                    bricks[ y * nBricksAcross + x ] = Brick( RectF( topLeft + Vec2( x * brickWidth, y * brickHeight ), brickWidth, brickHeight ), c );
    //                    nBricksLeft++;
    //                }
    //            }
    //        }
    //    }
    //}
}