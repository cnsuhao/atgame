#ifdef _ANDROID

#include <android_native_app_glue.h>
#include <cstdlib>

#include "GameLib.cpp"

/**
 * Main entry point.
 */
void android_main(struct android_app* state)
{
    // Android specific : Dummy function that needs to be called to 
    // ensure that the native activity works properly behind the scenes.
    app_dummy();

    Game game;
    if(game.Initialize(state))
    {
        game.Run();
        game.Shutdown();
    }
    // Android specific : the process needs to exit to trigger
    // cleanup of global and static resources (such as the game).
    exit(0);
}

#endif