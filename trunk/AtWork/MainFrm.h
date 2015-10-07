
// MainFrm.h : CMainFrame ��Ľӿ�
//

#pragma once
#include "ChildView.h"

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
    BOOL    IsEnterSizeMoved() { return m_EnterSizeMoved; }
    void    ViewRender();
    const Matrix& GetViewRotMatrix();

protected:  // �ؼ���Ƕ���Ա
	CStatusBar          m_wndStatusBar;
	CChildView          m_wndView;

    bool                m_EnterSizeMoved;

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
    afx_msg void OnEnterSizeMove();
    afx_msg void OnExitSizeMove();
	DECLARE_MESSAGE_MAP()
};

extern CMainFrame* g_MainFrame;
