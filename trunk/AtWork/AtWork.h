
// AtWork.h : AtWork Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CAtWorkApp:
// �йش����ʵ�֣������ AtWork.cpp
//

class CAtWorkApp : public CWinApp
{
public:
	CAtWorkApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
    virtual BOOL OnIdle(LONG lCount);
};

extern CAtWorkApp theApp;
