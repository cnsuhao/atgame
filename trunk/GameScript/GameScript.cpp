// GameScript.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "GameApp.h"

int _tmain(int argc, _TCHAR* argv[])
{
	GameApp game;
	if(game.Initialize(NULL))
	{
		game.Run();
		game.Shutdown();
	}
	return 0;
}

