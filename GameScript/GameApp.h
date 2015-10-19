#pragma once

#include "atgGame.h"

class GameApp : public atgGame
{
public:
	GameApp(void);
	~GameApp(void);

	virtual bool OnInitialize() OVERRIDE;

	virtual void OnShutdown() OVERRIDE;

	virtual void OnPause() OVERRIDE;

	virtual void OnResume() OVERRIDE;
protected:
	virtual void OnFrame() OVERRIDE;

	virtual void OnPointerDown(uint8 id, int16 x, int16 y) OVERRIDE;

	virtual void OnPointerMove(uint8 id, int16 x, int16 y) OVERRIDE;

	virtual void OnPointerUp(uint8 id, int16 x, int16 y) OVERRIDE;

	virtual void OnKeyScanDown(Key::Scan keyscan) OVERRIDE;

	void LoadConfig();
protected:
	//uint16 _mx;
	//uint16 _my;
	//bool   _down;
	//class atgCamera* _Camera;
};

