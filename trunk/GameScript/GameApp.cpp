#include "stdafx.h"
#include "GameApp.h"
#include "ScriptMng.h"

GameApp::GameApp(void)
{
}


GameApp::~GameApp(void)
{
}

bool GameApp::OnInitialize()
{
    ScriptMng::Get().Init();
	ScriptMng::Get().LoadScriptMain("scripts/main.js");
    ScriptMng::Get().Start();

	return true;
}

void GameApp::OnShutdown()
{
    ScriptMng::Get().End();
    ScriptMng::Get().Shutdown();
}

void GameApp::OnPause()
{

}

void GameApp::OnResume()
{
	
}

void GameApp::OnFrame()
{
	ScriptMng::Get().Update();
	::Sleep(1);
}

void GameApp::OnPointerDown( uint8 id, int16 x, int16 y )
{

}

void GameApp::OnPointerMove( uint8 id, int16 x, int16 y )
{

}

void GameApp::OnPointerUp( uint8 id, int16 x, int16 y )
{

}

void GameApp::OnKeyScanDown( Key::Scan keyscan )
{
    atgGame::OnKeyScanDown(keyscan);
}

void GameApp::LoadConfig()
{

}
