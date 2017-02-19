#include "KlingKlongManager.h"

KlingKlongManager::KlingKlongManager( Graphics& gfx_in, GameState& gameState_in )
    :
    gfx( gfx_in ),
    gameState( gameState_in )
{
}

void KlingKlongManager::Update( Keyboard& kbd )
{
    switch( gameState )
    {
    case GameState::START_SCREEN:
        UpdateStartScreen( kbd );
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
