#pragma once

#include "atgGame.h"
// TODO: 在此处引用程序需要的其他头文件
//#pragma comment(lib, "comctl32.lib")

class ViewerApp : public CWinApp,  public atgGame
{
public:
	ViewerApp(void);
	~ViewerApp(void);

	
	virtual bool            OnInitialize();

	virtual void            OnShutdown();

	virtual void            OnPause();

	virtual void            OnResume();

	virtual void            OnFrame();

	virtual void            OnPointerDown(uint8 id, int16 x, int16 y) override;

	virtual void            OnPointerMove(uint8 id, int16 x, int16 y) override;

	virtual void            OnPointerUp(uint8 id, int16 x, int16 y) override;

	virtual void			OnKeyScanDown(Key::Scan keyscan);

	virtual void			OnKeyScanUp(Key::Scan keyscan);

//protected:
	class FxbModel* pModel;
	class atgCamera* pCamera;
	class atgLight* pLight1;
    class atgLight* pLight2;

	bool leftBtnDown;
	bool rightBtnDown;
	bool middleBtnDown;

};

