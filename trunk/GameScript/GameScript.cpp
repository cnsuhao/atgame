// GameScript.cpp : �������̨Ӧ�ó������ڵ㡣
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

