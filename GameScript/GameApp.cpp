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
	ScriptMng::Get().Load("object.js");

	return true;
}

void GameApp::OnShutdown()
{

}

void GameApp::OnPause()
{

}

void GameApp::OnResume()
{
	
}

void GameApp::OnFrame()
{
	ScriptMng::Get().HotLoad();
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

}

void GameApp::LoadConfig()
{

}
