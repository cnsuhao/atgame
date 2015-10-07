#include "stdafx.h"
#include "MainView.h"
#include "GameApp.h"

CMainView::CMainView(GameApp& app):app(app)
{
}


CMainView::~CMainView(void)
{
}

void CMainView::OnInitialUpdate()
{
	g_hWnd = GetHwnd();
	CRect rect = GetWindowRect();
	g_Renderer = new atgRenderer(&app);
	g_Renderer->Initialize(rect.right - rect.left, rect.bottom - rect.top, 32);
	app.OnInitialize();
	SetTimer(1,1,NULL);
}

void CMainView::OnTime()
{
	app.OnFrame();
}

LRESULT CMainView::WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch (uMsg)
	{
	case WM_TIMER:	OnTime(); return TRUE;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		{
			Key::Scan scan = Virtualkey2KeyScan(wParam, lParam);
			app.OnKeyScanDown(scan);
		}
		break;
	case WM_KEYUP:
		{
			Key::Scan scan = Virtualkey2KeyScan(wParam, lParam);
			app.OnKeyScanUp(scan);
		}
		break;
	case WM_SYSKEYDOWN:
		{
			//LOG("wParam=%d, lParam=%d\n", wParam, lParam);
		}
		break;
	case WM_SYSKEYUP:
		{

		}
		break;
	case WM_MOUSEMOVE:
		{
			int x_pos = LOWORD(lParam);
			int y_pos = HIWORD(lParam);
			app.OnPointerMove(MBID_UNKNOWN, x_pos, y_pos);
			return TRUE;
			return 0;
		}
		break;
	case WM_LBUTTONDOWN:
		{
			int x_pos = LOWORD(lParam);
			int y_pos = HIWORD(lParam);
			app.OnPointerDown(MBID_LEFT, x_pos, y_pos);
			return TRUE;
		}
		break;
	case WM_LBUTTONUP:
		{
			int x_pos = LOWORD(lParam);
			int y_pos = HIWORD(lParam);
			app.OnPointerUp(MBID_LEFT, x_pos, y_pos);
			return TRUE;
		}
		break;
	case WM_RBUTTONDOWN:
		{
			int x_pos = LOWORD(lParam);
			int y_pos = HIWORD(lParam);
			app.OnPointerDown(MBID_RIGHT, x_pos, y_pos);
			return TRUE;
		}
		break;
	case WM_RBUTTONUP:
		{
			int x_pos = LOWORD(lParam);
			int y_pos = HIWORD(lParam);
			app.OnPointerUp(MBID_RIGHT, x_pos, y_pos);
			return TRUE;
		}
		break;
	case WM_MBUTTONDOWN:
		{
			int x_pos = LOWORD(lParam);
			int y_pos = HIWORD(lParam);
			app.OnPointerDown(MBID_MIDDLE, x_pos, y_pos);
			return TRUE;
		}
		break;
	case WM_MBUTTONUP:
		{
			int x_pos = LOWORD(lParam);
			int y_pos = HIWORD(lParam);
			app.OnPointerUp(MBID_MIDDLE, x_pos, y_pos);
			return TRUE;
		}
		break;
	case WM_MOUSEWHEEL:
		{
			int16 wheel = (int16)HIWORD(wParam) / WHEEL_DELTA;
			if (wheel > 0)
				app.OnPointerMove(MBID_MIDDLE, 1, 0);

			else
				app.OnPointerMove(MBID_MIDDLE, 0, 1);
			return TRUE;
		}
		break;
	case WM_CHAR:
		break;
	case WM_SIZE:{
		//CRect rect = GetWindowRect();
		//g_Renderer->SetViewport(0,0, rect.right - rect.left, rect.bottom - rect.top);
		//float aspect = (rect.right - rect.left) * 1.0f / (rect.bottom - rect.top) * 1.0f;
		//app.pCamera->SetProjection(60.0f, aspect, 0.1f, 1000.0f);
		}break;
	default:break;
	}

	return WndProcDefault(uMsg, wParam, lParam);
}
