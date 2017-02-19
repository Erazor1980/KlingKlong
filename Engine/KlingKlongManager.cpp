#include "KlingKlongManager.h"

KlingKlongManager::KlingKlongManager( Graphics& gfx_in, GameState& gameState_in )
    :
    gfx( gfx_in ),
    gameState( gameState_in )
{
    startTime_startScreen = std::chrono::steady_clock::now();
}

void KlingKlongManager::Update()
{
    switch( gameState )
    {
    case GameState::START_SCREEN:
    {
        const std::chrono::duration<float> timeElapsed = std::chrono::steady_clock::now() - startTime_startScreen;
    }
    break;
    default:
        break;
    }
}

void KlingKlongManager::DrawScene() const
{
    switch( gameState )
    {
    case GameState::START_SCREEN:
    {
        int x = gfx.ScreenWidth / 2 - WelcomeTo.GetWidth() / 2;
        int y = gfx.ScreenHeight / 5;
        gfx.DrawSpriteKey( x, y, WelcomeTo, WelcomeTo.GetPixel( 0, 0 ) );
        const std::chrono::duration<float> timeElapsed = std::chrono::steady_clock::now() - startTime_startScreen;
        x = gfx.ScreenWidth / 2 - KlingKlong.GetWidth() / 2;
        y = gfx.ScreenHeight / 3;
        if( timeElapsed.count() > 1 && timeElapsed.count() < 2 )
        {
            gfx.DrawSpriteKeyFromSequence( x, y, KlingKlong, KlingKlong.GetPixel( 0, 0 ), 0, 2, 1 );
        }
        else if( timeElapsed.count() >= 2 )
        {
            gfx.DrawSpriteKey( x, y, KlingKlong, KlingKlong.GetPixel( 0, 0 ) );
        }        
    }
    break;
    default:
        break;
    }
}
