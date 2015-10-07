#include "stdafx.h"
#include "GameApp.h"

GameApp::GameApp(void):m_Frame(*this)
{
}


GameApp::~GameApp(void)
{
}


bool GameApp::OnInitialize()
{
	g_Renderer->SetViewport(0, 0, GetWindowWidth(), GetWindowHeight());
	g_Renderer->SetFaceCull(FCM_CW);

	pCamera = new atgCamera();
	g_Renderer->SetMatrix(MD_VIEW, pCamera->GetView().m);

	g_Renderer->SetGlobalAmbientColor(Vec3One);

	return true;
}

void            GameApp::OnShutdown()
{

}

void            GameApp::OnPause()
{

}

void            GameApp::OnResume()
{

}

void            GameApp::OnFrame()
{
	g_Renderer->Clear();

	g_Renderer->BeginFrame();

	g_Renderer->EndFrame();

	g_Renderer->Present();
}

void            GameApp::OnPointerDown(uint8 id, int16 x, int16 y)
{

}

void            GameApp::OnPointerMove(uint8 id, int16 x, int16 y)
{

}

void            GameApp::OnPointerUp(uint8 id, int16 x, int16 y)
{
}

void			GameApp::OnKeyScanDown(Key::Scan keyscan)
{

}

void			GameApp::OnKeyScanUp(Key::Scan keyscan)
{
}

// CWinApp
BOOL			GameApp::InitInstance()
{
	//Create the Frame Window
	if (m_Frame.Create() == 0)	
	{
		// We get here if the Frame creation fails

		::MessageBox(NULL, _T("Failed to create Frame window"), _T("ERROR"), MB_ICONERROR);
		return FALSE; // returning FALSE ends the application
	}
	return TRUE;
}