// GameScript.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "GameApp.h"
#include "atgGame.h"

//#include <io.h>
//#include <fcntl.h>

int _tmain(int argc, _TCHAR* argv[])
{
    //_setmode(_fileno(stdout), _O_U8TEXT); 导致脚本出错.注解掉了    
    GameApp game;
	if(game.Initialize(NULL))
	{
		game.Run();
		game.Shutdown();
	}
	return 0;
}

