// GameFrame.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "GameFrame.h"
#include "GameApp.h"

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

CMainFrame::CMainFrame(GameApp& app):app(app),m_View(app)
{
	SetView(m_View);
}
CMainFrame::~CMainFrame()
{

}

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	wmId    = LOWORD(wParam);
	wmEvent = HIWORD(wParam);
	// 分析菜单选择:
	switch (wmId)
	{
	case IDM_ABOUT:
		DialogBox(GetApp()->GetInstanceHandle(), MAKEINTRESOURCE(IDD_ABOUTBOX), GetHwnd(), About);
		break;
	case IDM_EXIT:
		DestroyWindow(GetHwnd());
		break;
	default:
		break;
	}

	
	return CWnd::OnCommand(wParam, lParam);
}
int	CMainFrame::OnCreate(LPCREATESTRUCT pcs)
{
	//m_UseIndicatorStatus = FALSE;	// Don't show keyboard indicators in the StatusBar
	//m_UseMenuStatus = FALSE;			// Don't show menu descriptions in the StatusBar
	//m_UseReBar = FALSE;				// Don't use a ReBar
	//m_UseThemes = FALSE;				// Don't use themes
	m_UseToolBar = FALSE;			// Don't use a ToolBar

	// call the base class function
	return CFrame::OnCreate(pcs);
}

void CMainFrame::OnInitialUpdate()
{
	// center the window
	CRect rect = GetWindowRect();
	int nWndWidth = rect.right - rect.left;
	int nWndHeight = rect.bottom - rect.top;

	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	rect.left = (nScreenWidth - nWndWidth) / 2;
	rect.top = (nScreenHeight - nWndHeight) / 2;

	SetWindowPos(NULL, rect.left, rect.top, nWndWidth, nWndHeight, SWP_SHOWWINDOW);
}
//virtual LRESULT OnNotify(WPARAM wParam, LPARAM lParam);
//virtual void	PreCreate(CREATESTRUCT &cs);
//virtual void	SetupToolBar();
LRESULT CMainFrame::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CWnd::WndProc(uMsg, wParam, lParam);
}

void CMainFrame::OnTime()
{
	app.OnFrame();
}

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// Start Win32++
	GameApp MyApp;

	// Run the application
	return MyApp.CWinApp::Run();
}