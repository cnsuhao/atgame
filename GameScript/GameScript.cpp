// GameScript.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "GameApp.h"
#include "atgGame.h"

//#include <io.h>
//#include <fcntl.h>

int _tmain(int argc, _TCHAR* argv[])
{
    //_setmode(_fileno(stdout), _O_U8TEXT); ���½ű�����.ע�����    
    GameApp game;
	if(game.Initialize(NULL))
	{
		game.Run();
		game.Shutdown();
	}
	return 0;
}

