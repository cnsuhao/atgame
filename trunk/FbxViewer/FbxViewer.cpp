// FbxViewer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ViewerApp.h"
#include "atgRenderer.h"

class FxbViewer : public CWnd
{
public:
	FxbViewer(ViewerApp& app):app(app) {}
    virtual ~FxbViewer() {}
    virtual void OnDestroy() { PostQuitMessage(0); }	// Ends the application


	virtual void OnInitialUpdate();
	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnTime();
private:
	ViewerApp& app;
};

void FxbViewer::OnInitialUpdate()
{
	g_hWnd = GetHwnd();
	CRect rect = GetWindowRect();
	g_Renderer = new atgRenderer(NULL);
	g_Renderer->Initialize(rect.right - rect.left, rect.bottom - rect.top, 32);
	app.OnInitialize();
	SetTimer(1,1,NULL);
}

LRESULT FxbViewer::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_TIMER:	OnTime(); return TRUE;
    case WM_CREATE:
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
        break;
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
            return 0;
        }
        break;
    case WM_LBUTTONDOWN:
        {
            int x_pos = LOWORD(lParam);
            int y_pos = HIWORD(lParam);
            app.OnPointerDown(MBID_LEFT, x_pos, y_pos);
        }
        break;
    case WM_LBUTTONUP:
        {
            int x_pos = LOWORD(lParam);
            int y_pos = HIWORD(lParam);
			app.OnPointerUp(MBID_LEFT, x_pos, y_pos);
        }
        break;
    case WM_RBUTTONDOWN:
        {
            int x_pos = LOWORD(lParam);
            int y_pos = HIWORD(lParam);
			app.OnPointerDown(MBID_RIGHT, x_pos, y_pos);
        }
        break;
    case WM_RBUTTONUP:
        {
            int x_pos = LOWORD(lParam);
            int y_pos = HIWORD(lParam);
			app.OnPointerUp(MBID_RIGHT, x_pos, y_pos);
        }
        break;
    case WM_MBUTTONDOWN:
        {
            int x_pos = LOWORD(lParam);
            int y_pos = HIWORD(lParam);
			app.OnPointerDown(MBID_MIDDLE, x_pos, y_pos);
        }
        break;
    case WM_MBUTTONUP:
        {
            int x_pos = LOWORD(lParam);
            int y_pos = HIWORD(lParam);
			app.OnPointerUp(MBID_MIDDLE, x_pos, y_pos);
        }
        break;
    case WM_MOUSEWHEEL:
        {
            int16 wheel = (int16)HIWORD(wParam) / WHEEL_DELTA;
            if (wheel > 0)
                app.OnPointerMove(MBID_MIDDLE, 1, 0);
    
            else
				app.OnPointerMove(MBID_MIDDLE, 0, 1);
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

	return CWnd::WndProc(uMsg, wParam, lParam);
}

void FxbViewer::OnTime()
{
	app.OnFrame();
}

int _tmain(int argc, _TCHAR* argv[])
{
	ViewerApp app;
	g_hInstance = app.GetInstanceHandle();
	//if(app.Initialize(NULL))
	//{
	//	app.Run();
	//	app.Shutdown();
	//}
	FxbViewer viewer(app);
	viewer.Create();

	app.CWinApp::Run();

	return 0;
}

