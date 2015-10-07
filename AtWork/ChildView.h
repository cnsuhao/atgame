
// ChildView.h : CChildView ��Ľӿ�
//


#pragma once
#include "atltypes.h"


// CChildView ����

class CChildView : public CWnd
{
// ����
public:
	CChildView();

// ����
public:
    void InitResource();
    void ResetResource();
    void Render();
    void RenderAxis();
    const Matrix& GetRotMatrix();
// ����
public:

// ��д
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ʵ��
public:
	virtual ~CChildView();

	// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
 
protected:
    bool m_RandererInit;
    bool m_EnterSizeMove;
    bool m_LbuttonDown;
    CPoint m_lastPoint;
    Matrix m_RotMat;

    atgVertexBuffer* m_VB;
    atgPass*         m_PassColor;
    atgPass*         m_PassTexture;
    atgTexture*      m_Texture;
    atgCamera*       m_Camera;
public:
    afx_msg void OnDestroy();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

extern HWND g_hWnd;