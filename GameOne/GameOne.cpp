// GameOne.cpp : �������̨Ӧ�ó������ڵ㡣
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

