// GameOne.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Game.h"

int _tmain(int argc, _TCHAR* argv[])
{

    Game game;
    if(game.Initialize(NULL))
    {
        game.Run();
        game.Shutdown();
    }

	return 0;
}

