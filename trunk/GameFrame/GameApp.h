#pragma once

#include "GameFrame.h"

class GameApp : public CWinApp,  public atgGame
{
public:
	GameApp(void);
	~GameApp(void);
	// atgGame
	virtual bool            OnInitialize() override;

	virtual void            OnShutdown() override;

	virtual void            OnPause() override;

	virtual void            OnResume() override;

	virtual void            OnFrame() override;

	virtual void            OnPointerDown(uint8 id, int16 x, int16 y) override;

	virtual void            OnPointerMove(uint8 id, int16 x, int16 y) override;

	virtual void            OnPointerUp(uint8 id, int16 x, int16 y) override;

	virtual void			OnKeyScanDown(Key::Scan keyscan) override;

	virtual void			OnKeyScanUp(Key::Scan keyscan) override;
	// CWinApp
	virtual BOOL			InitInstance();
protected:
	CMainFrame				m_Frame;
	atgCamera*				pCamera;
};

