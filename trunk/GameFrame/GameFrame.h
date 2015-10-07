#pragma once

#include "resource.h"

#include "MainView.h"

class GameApp;

class CMainFrame : public CFrame
{
public:
	CMainFrame(GameApp& app);
	virtual ~CMainFrame();

protected:
	virtual BOOL	OnCommand(WPARAM wParam, LPARAM lParam);
	virtual int		OnCreate(LPCREATESTRUCT pcs);
	virtual void	OnInitialUpdate();
	//virtual LRESULT OnNotify(WPARAM wParam, LPARAM lParam);
	//virtual void	PreCreate(CREATESTRUCT &cs);
	//virtual void	SetupToolBar();
	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnTime();
private:
	GameApp& app;
	CMainView m_View;
};