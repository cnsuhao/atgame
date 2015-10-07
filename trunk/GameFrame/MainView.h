#pragma once

class GameApp;
class CMainView : public CWnd
{
public:
	CMainView(GameApp& app);
	~CMainView(void);
	virtual void	OnInitialUpdate();
	virtual void	OnDestroy() { PostQuitMessage(0); }	// Ends the application
	void			OnTime();
	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	GameApp& app;
};

